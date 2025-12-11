public class HLL {
    // below variables need to be serialized
    int p;
    int r;
    byte[] registers;

    // below variables are derived
    int m;

    // below are constants
    static final byte BIT_MASK_2 = 3;
    static final byte BIT_MASK_4 = 15;
    static final byte BIT_MASK_6 = 63;
    static final byte BIT_MASK_8 = (byte) 255;
    static final double POW_2_32 = Math.pow(2, 32);
    static double[] PRE_POW_2_K = new double[256];
    static final int DT_WIDTH = 8;
    static {
        for(int i = 0; i< 256; i++) {
            PRE_POW_2_K[i] = Math.pow(2, -i);
        }
    }

    public HLL(int p, int r) {
        assert(p >= 4 && p <= 30);
        assert(r == 4 || r == 6 || r == 8);

        this.p = p;
        this.r = r;
        this.m = r * (1 << p) >> 3;
        this.registers = new byte[m];
    }

    private HLL(byte[] array) {
        this.m = array.length - 2;
        this.p = array[m];
        this.r = array[m + 1];

        this.registers = new byte[m];
        for(int i=0; i<m; i++)
            this.registers[i] = array[i];
    }

    // read r bits of the registers from a specified bit location and return it as a byte.
    byte readRegister(int index) {
        byte value = 0;
        int end = index + r - 1;
        int firstByteIndex = index >>> 3;
        int secondByteIndex = end >>> 3;
        int lastBitOffset = 7 - (end & 7);

        if (firstByteIndex == secondByteIndex) {
            // bits packed in single byte
            byte MASK = (r == 4) ? BIT_MASK_4 : ((r == 6) ? BIT_MASK_6 : BIT_MASK_8);
            return (byte) ((this.registers[firstByteIndex] >>> lastBitOffset) & MASK);
        }

        // bits packed in two bytes
        int firstBitOffset = 7 - (index & 7);
        // as we only support r = 6 which can be misaligned no need to check other things
        byte MASK_1 = (firstBitOffset == 1) ? BIT_MASK_2 : BIT_MASK_4;
        byte MASK_2 = (lastBitOffset == 6) ? BIT_MASK_2 : BIT_MASK_4;
        value = (byte) ((this.registers[firstByteIndex] & MASK_1) << ((end & 7) + 1));
        value = (byte) (value | (this.registers[secondByteIndex] >>> lastBitOffset) & MASK_2);
        return value;
    }

    // write the rightmost r bits of given byte value to a specified bit location in the registers.
    void writeRegister(byte value, int index) {
        int end = index + r - 1;
        int firstByteIndex = index >>> 3;
        int secondsByteIndex = end >>> 3;
        int lastBitOffset = 7 - (end & 7);

        if(firstByteIndex == secondsByteIndex) {
            // bits packed in same byte
            byte MASK = (r == 4) ? BIT_MASK_4 : ((r == 6) ? BIT_MASK_6 : BIT_MASK_8);
            this.registers[firstByteIndex] &= (byte) ~(MASK << lastBitOffset);
            this.registers[firstByteIndex] |= (byte) (value << lastBitOffset);
            return;
        }

        // bits packed in two bytes
        int firstBitOffset = 7 - (index & 7);
        byte MASK_1 = (firstBitOffset == 1) ? BIT_MASK_2 : BIT_MASK_4;
        byte MASK_2 = (lastBitOffset == 6) ? BIT_MASK_2 : BIT_MASK_4;

        this.registers[firstByteIndex] &= (byte) ~MASK_1;
        this.registers[firstByteIndex] |= (byte) (value >>> ((end & 7) + 1));
        this.registers[secondsByteIndex] &= (byte) ~(MASK_2 << lastBitOffset);
        this.registers[secondsByteIndex] |= (byte) (value << lastBitOffset);
    }

    public void add(long value) {
        int bucket = (int) (value >>> (64 - p));
        value = value | 1L << (64 - p);
        int cnt = Long.numberOfTrailingZeros(value) + 1;

        int maxBucketValue = ((1 << r) - 1);
        if(cnt > maxBucketValue)
            cnt = (byte) maxBucketValue;

        int bucketBitPosition = bucket * r;

        int prev = readRegister(bucketBitPosition) & 0xFF;
        if(prev < cnt)
            writeRegister((byte) cnt, bucketBitPosition);
    }

    public void merge(HLL other) {
        assert(this.m == other.m);

        int M = (m * DT_WIDTH) / r;
        for(int i = 0; i< M; i++) {
            int a = this.readRegister(i * r) & 0xFF;
            int b = other.readRegister(i * r) & 0xFF;
            if(a < b) {
                this.writeRegister((byte) b, i * r);
            }
        }
    }

    public long estimate() {
        double M = (double) (m * DT_WIDTH) / r;
        double sum = 0;
        double zeroRegisters = 0;
        for(int i=0; i<M; i++) {
            int k = readRegister(i * r) & 0xFF;
            if(k == 0)
                zeroRegisters++;
            sum = sum + PRE_POW_2_K[k];
        }
        double alphaM = getAlphaM((int) M);
        double rawEstimate = alphaM * M * M * (1 / sum);
//        System.err.println(rawEstimate);

        if(rawEstimate <= (5.0 * M / 2.0) && zeroRegisters > 0) {
            rawEstimate = M * Math.log(M / zeroRegisters);
        } else if(rawEstimate > ((1.0 / 30.0) * POW_2_32)) {
            rawEstimate = - POW_2_32 * Math.log(1 - rawEstimate / POW_2_32);
        }

        return (long) rawEstimate;
    }

    private double getAlphaM(int M) {
        switch (M) {
            case 16:
                return 0.673;
            case 32:
                return 0.697;
            case 64:
                return 0.709;
            default:
                return 0.7213 / (1 + (1.079 / M));
        }
    }

    public byte[] serialize() {
        int size = m + 2;

        byte[] array = new byte[size];
        for(int i=0; i<m; i++)
            array[i] = registers[i];

        array[m] = (byte) p;
        array[m + 1] = (byte) r;
        return array;
    }

    public static HLL deserialize(byte[] array) {
        return new HLL(array);
    }

    void debugInfo() {
        int M = (m * DT_WIDTH) / r;
        System.err.println("p=" + p + " r=" + r + " bytes=" + m + " M=" + M);
        int max = (1 << r) - 1;
        int[] hist = new int[max + 1];
        for (int i = 0; i < M; i++) {
            int v = readRegister(i * r) & 0xFF;
            hist[v]++;
        }
        for (int i = 0; i <= max; i++)
            System.err.printf("reg %2d: %6d%n", i, hist[i]);
        double zero = hist[0];
        System.err.println("zeroRegs=" + zero);
    }
}