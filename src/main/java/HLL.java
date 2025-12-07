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

    HLL(int p, int r) {
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

//        for(int i=0; i < r; i++) {
//            int byteIndex = (index + i) / 8;
//            int bitIndex = (index + i) % 8;
//            byte bit = (byte) ((registers[byteIndex] >>> (7 - bitIndex)) & 1);
//            value = (byte) ((value << 1) | bit);
//        }

        int end = index + r - 1;
        int firstByte = index / 8;
        int secondByte = end / 8;

        int secondBitOffset = 7 - (end % 8);

        if(firstByte == secondByte) {
            // bits packed in single byte
            byte MASK = (r == 4) ? BIT_MASK_4 : ((r == 6) ? BIT_MASK_6 : BIT_MASK_8);
            value = (byte) ((this.registers[firstByte] >>> secondBitOffset) & MASK);
        }
        else {
            // bits packed in two bytes
            int firstBitOffset = 7 - (index % 8);
            // as we only support r = 6 which can be misaligned no need to check other things
            byte MASK_1 = (firstBitOffset == 2) ? BIT_MASK_2 : BIT_MASK_4;
            byte MASK_2 = (secondBitOffset == 6) ? BIT_MASK_2 : BIT_MASK_4;
            value = (byte) ((this.registers[firstByte] & MASK_1) << ((end % 8) + 1));
            value = (byte) (value | (this.registers[secondByte] >>> secondBitOffset) & MASK_2);
        }
        return value;
    }

    // write the rightmost r bits of given byte value to a specified bit location in the registers.
    void writeRegister(byte value, int index) {
        for(int i= r-1; i>=0; i--) {
            int byteIndex = (index + i) / 8;
            int bitIndex = (index + i) % 8;
            byte bit = (byte) (value & 1);
            registers[byteIndex] &= (byte) ~(1 << (7 - bitIndex));
            registers[byteIndex] |= (byte) (bit << (7 - bitIndex));
            value = (byte) (value >>> 1);
        }
    }

    void add(long value) {
        byte cnt = 0;

        int bucket = (int) (value >>> (64 - p));
        value = (value << p) >>> p;
        while(value > 0) {
            cnt++;
            if((value & 1) == 1)
                break;
            value = value >>> 1;
        }

        int maxBucketValue = ((1 << r) - 1);
        if(cnt > maxBucketValue)
            cnt = (byte) maxBucketValue;

        int bucketBitPosition = bucket * r;

        byte prev = readRegister(bucketBitPosition);
        if(prev < cnt)
            writeRegister(cnt, bucketBitPosition);
    }

    void merge(HLL other) {
        assert(this.m == other.m);

        int totalBuckets = (m * 8) / r;
        for(int i=0; i<totalBuckets; i++) {
            byte a = this.readRegister(i * r);
            byte b = other.readRegister(i * r);
            if(a < b) {
                this.writeRegister(b, i * r);
            }
        }
    }

    long estimate() {
        double M = (double) (m * 8) / r;
        double sum = 0;
        double zeroRegisters = 0;
        for(int i=0; i<M; i++) {
            double k = readRegister(i * r);
            if(k == 0)
                zeroRegisters++;
            sum = sum + Math.pow(2, -k);
        }
        double alphaM = getAlphaM((int) M);

        if(M == 16)
            alphaM = 0.673;
        else if (M == 32) {
            alphaM = 0.697;
        } else if (M == 64) {
            alphaM = 0.709;
        }

        double rawEstimate = alphaM * M * M * (1 / sum);
//        System.err.println(rawEstimate);

        if(rawEstimate <= (5.0 * M / 2.0) && zeroRegisters > 0) {
            rawEstimate = M * Math.log(M / zeroRegisters);
        } else if(rawEstimate > ((1.0 / 30.0) * Math.pow(2, 32))) {
            rawEstimate = -Math.pow(2, 32) * Math.log(1 - rawEstimate / Math.pow(2, 32));
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

    byte[] serialize() {
        int size = m + 2;

        byte[] array = new byte[size];
        for(int i=0; i<m; i++)
            array[i] = registers[i];

        array[m] = (byte) p;
        array[m + 1] = (byte) r;
        return array;
    }

    static HLL deserialize(byte[] array) {
        return new HLL(array);
    }

    void debugInfo() {
        int M = (m * 8) / r;
        System.err.println("p=" + p + " r=" + r + " bytes=" + m + " M=" + M);
        int max = (1 << r) - 1;
        int[] hist = new int[max + 1];
        for (int i = 0; i < M; i++) {
            int v = readRegister(i * r);
            hist[v]++;
        }
        for (int i = 0; i <= max; i++)
            System.err.printf("reg %2d: %6d%n", i, hist[i]);
        double zero = hist[0];
        System.err.println("zeroRegs=" + zero);
    }
}
