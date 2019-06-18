 public class test{
         public static void main(String[] args){
        System.out.println("Hello World!");
        byte[] b={0x41,0xf5,0x1e,0xb6};

        byte2float(b,0);

        //gitsdadad Culpa eiusmod elit officia est pariatur. Fugiat sit est sit dolore qui exercitation pariatur. Aute fugiat ipsum ex dolor est fugiat qui incididunt non officia id. Reprehenderit ipsum nostrud aliquip do tempor enim proident aliqua dolore cupidatat. Laboris occaecat elit 
    }
    public static float byte2float(byte[] b, int index) {
        int l;
        l = b[index + 0];
        l &= 0xff;
        l |= ((long) b[index + 1] << 8);
        l &= 0xffff;
        l |= ((long) b[index + 2] << 16);
        l &= 0xffffff;
        l |= ((long) b[index + 3] << 24);
        return Float.intBitsToFloat(l);
    }
 }
 