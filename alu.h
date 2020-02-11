#ifndef ALU_H
#define ALU_H


class Alu
{
public:
    Alu(unsigned int sa, unsigned int ea, unsigned int fa, unsigned int sb, unsigned int eb, unsigned int fb);
    void suma();
    void multiplica();
    void divide();
    unsigned int multiplicacionBinariaSinSigno(unsigned int n1, unsigned int n2);
    void generaMantisas();
    unsigned int getSigno();
    unsigned int getExponente();
    unsigned int getFraccionaria();
private:
    unsigned int sa;
    unsigned int ea;
    unsigned int fa;
    unsigned int ma;
    unsigned int sb;
    unsigned int eb;
    unsigned int fb;
    unsigned int mb;
    unsigned int sr;
    unsigned int er;
    unsigned int fr;
    unsigned int mr;
    struct campos{
         unsigned int f:23;
         unsigned int e:8;
         unsigned int s:1;
    };

    union valores{
         float real;
         unsigned int entero;
         struct campos bits ;
    };
};

#endif // ALU_H
