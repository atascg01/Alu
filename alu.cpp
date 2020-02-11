#include "alu.h"
#include <math.h>
Alu::Alu(unsigned int sa, unsigned int ea, unsigned int fa, unsigned int sb, unsigned int eb, unsigned int fb){
    this->sa=sa;
    this->ea=ea;
    this->fa=fa;
    this->sb=sb;
    this->eb=eb;
    this->fb=fb;
    generaMantisas();
}

unsigned int Alu::getSigno(){
    return this->sr;
}

unsigned int Alu::getExponente(){
    return this->er;
}

unsigned int Alu::getFraccionaria(){
    return this->fr;
}

void Alu::generaMantisas(){
    if(ea!=0){
        ma = (fa | 0x800000);
    }
    else{
        ma = fa;
    }
    if(eb!=0){
        mb = (fb | 0x800000);
    }
    else{
        mb = fb;
    }
}

void Alu::suma(){

    /* COMPROBACION CASOS ESPECIALES */

    if((fa!=0 && ea == 255) || (fb!=0 && eb == 255)){ //Numero NaN
        er = 255;
        sr = 0;
        fr = 1;
        return;
    }
    if((sa == 0 && sb == 0) && (ea == 255 && eb == 255)){ //Dos infinitos positivos
        er = 255;
        sr = 0;
        fr = 0;
        return;
    }

    if((sa == 1 && sb == 1) && (ea == 255 && eb == 255)){ //Dos infinitos negativos
        er = 255;
        sr = 1;
        fr = 0;
        return;
    }
    if((sa == 0 && ea == 255) || (sb == 0 && eb == 255)){ //Numero mas infinito positivo
        er = 255;
        sr = 0;
        fr = 0;
        return;
    }
    if((sa == 1 && ea == 255) || (sb == 1 && eb == 255)){ //Numero mas infinito negativo
        er = 255;
        sr = 1;
        fr = 0;
        return;
    }

    /* ////////////////// PASO 1 ////////////////// */

    unsigned int P=0;
    unsigned int g=0;
    unsigned int r=0;
    unsigned int st=0;
    unsigned int d=-1;
    unsigned int n=24;
    bool Operandos_intercambiados=false;
    bool Complementado_P=false;

    if( sa == 1 && ea==0 && fa==0){
        sa = 0;
    }
    if( sb == 1 && eb==0 && fb==0){
        sb = 0;
    }
    if(ea==0){
        ma = (ma & 0x7fffff);
//        ma = fa;
//        ea=-126;
    }
    if(eb==0){
        mb = (mb & 0x7fffff);
//        mb = fb;
//        eb=-126;
    }

    /* ////////////////// PASO 2 ////////////////// */

    if(ea<eb){
        unsigned int intercambio=ea;
        ea=eb;
        eb=intercambio;
        intercambio=ma;
        ma=mb;
        mb=intercambio;
        intercambio=sa;
        sa=sb;
        sb=intercambio;
        intercambio=fa;
        fa=fb;
        fb=intercambio;
        Operandos_intercambiados=true;
    }

    /* ////////////////// PASO 3 ////////////////// */

    unsigned int es=ea;
    if(ea-eb>=0){
        d = ea-eb;
    }
    else{
        d = 0;
    }

    /* ////////////////// PASO 4 ////////////////// */

    if(sa!=sb){
        mb=-mb;
        mb = (mb & 0xffffff);
    }

    /* ////////////////// PASO 5 ////////////////// */

    P=mb;

    /* ////////////////// PASO 6 ////////////////// */

    if(d>=1){
        unsigned int P2=P;
        P2 = P2 >> (d-1);
        g = (P2 & 0x000001);
    }
    if(d>=2){
        unsigned int P2=P;
        P2 = P2 >> (d-1);
        r = (P2 & 0x000002);
    }
    if(d>=1){  //STICKY
        unsigned int sticky1 = (P & 0x000001);
        unsigned int P2=P;
        for(int i=0;i<d;i++){
            P2 = P2 >> 1;
            unsigned int sticky2 = (P2 & 0x000001);
            st = sticky1 | sticky2;
            sticky1 = sticky2;
        }
    }
    if(d==0){
        st = (P & 0x1);
    }

    /* ////////////////// PASO 7 ////////////////// */

    if(d>=0){
        if(sa!=sb){
            for(int i=0;i<d;i++){
                P = P >> 1;
                P = (P | 0x800000);
            }
        }
        else{
            for(int i=0;i<d;i++){
                P = P >> 1;
                P = (P | 0x000000);
            }
        }
    }

    /* ////////////////// PASO 8 ////////////////// */

    P+=ma;
    unsigned int P2=P;
    P2 = P2 >> n;
    unsigned int c1 = (P2 & 0x000001);

    if(c1==1){ //Quito bit 24 de la mantisa (acarreo)
        P = P-0x1000000;
    }

    /* ////////////////// PASO 9 ////////////////// */

    unsigned int P3=P;
    P3 = P3 >> (n-1);
    unsigned int bit=(P3 & 0x000001);
    if( (sa!=sb) && (bit==1) && (c1==0)){
        P=-P;
        P = (P & 0xffffff);
        Complementado_P=true;
    }

    /* ////////////////// PASO 10 ////////////////// */

    if((sa==sb) && (c1==1)){
        st = (g | r | st);
        r = (P & 0x000001);
        c1 = c1 >> 1;
        P = P >> 1;
        es+=1;
    }
    else{
        unsigned int k=0;
        unsigned int P2 = P;
        for(int i=23;i>=0;i--){
            if((P2 >> i & 0x01) == 0){
                k++;
            }
            else{
                break;
            }
        }
        if(k==24){
            this->sr=0;
            this->er=0;
            this->fr=0;
            return;
        }
        if(k==0){
            st=(r | st);
            r=g;
        }
        if(k>=1){
            r=0;
            st=0;
        }
        P = P << k;
        g = g << k;
        es-=k;
    }

    /* ////////////////// PASO 11 ////////////////// */

    unsigned int ms;
    if(((r==1) && (st==1)) || ((r==1) && (st==0) && ((P & 0x000001)==1))){
        P+=1;
        unsigned int P2=P;
        for(int i=0;i<n;i++){
            P2 = P2 >> 1;
        }
        unsigned int c2 = (P2 & 0x000001);
        if(c2==1){
            c2 = c2 >> 1;
            P = P >> 1;
            es+=1;
        }
    }
    ms=P;

    /* ////////////////// PASO 12 ////////////////// */

    unsigned int ss;
    if(!Operandos_intercambiados && Complementado_P){
        ss=sb;
    }
    else{
        ss=sa;
    }

    /* ////////////////// PASO 13 ////////////////// */

//    this->resultado = ss*ms*pow(2,es);
    this->sr=ss;
    this->er=es;
    this->fr=ms-0x1000000;

    /* ////////////////// TERMINADO ////////////////// */

}

unsigned int Alu::multiplicacionBinariaSinSigno(unsigned int n1, unsigned int n2){

    /* ////////////////// PASO 1 ////////////////// */

    unsigned int A = n1;
    unsigned int B = n2;
    unsigned int n = 24;
    unsigned int P = 0;

    /* ////////////////// PASO 2 ////////////////// */

    unsigned int c1 = 0;
    for(int i = 0; i<n;i++){
        if( ( A & 0x1 ) == 1 ){
            P+=B;
            unsigned int P2=P;
            for(int i=0;i<n;i++){
                P2 = P2 >> 1;
            }
            c1 = (P2 & 0x1);

            if(c1==1){ //Quito bit 24 (acarreo)
                P = P-0x1000000;
            }

            unsigned int bitp = P & 0x1;
            P = P >> 1;
            if(c1==1){
                P = P | 0x00800000;
            }
            A = A >> 1;
            if(bitp==1){
                A = A | 0x00800000;
            }
        }
        else{
            P+=0;
            c1 = 0;
            unsigned int bitp = P & 0x1;
            P = P >> 1;
            if(c1==1){
                P = P | 0x800000;
            }
            A = A >> 1;
            if(bitp==1){
                A = A | 0x800000;
            }
        }
    }

    /* ////////////////// PASO 3 ////////////////// */

    return P;

}

void Alu::multiplica(){

    /* COMPROBACION CASOS ESPECIALES */

    if((fa!=0 && ea == 255) || (fb!=0 && eb == 255)){ //Numero NaN
        er = 255;
        sr = 0;
        fr = 1;
        return;
    }
    if( (ea == 0 && eb == 255 && sb == 0) || (sa == 0 && ea == 255 && eb == 0)){ // 0 por infinito positivo
        sr = 0;
        er = 255;
        fr = 8388607;
        return;
    }
    if( (ea == 0 && eb == 255 && sb == 1) || (sa == 1 && ea == 255 && eb == 0)){ // 0 por infinito negativo
        sr = 1;
        er = 255;
        fr = 0;
        return;
    }
    if( ea == 0 || eb == 0){ //Dos ceros
        fr = 0;
        er = 0;
        sr = 0;
        return;
    }
    if((sa == 0 && sb == 0) && (ea == 255 && eb == 255)){ //Dos infinitos positivos
        er = 255;
        sr = 0;
        fr = 0;
        return;
    }
    if((sa == 1 && sb == 0) && (ea == 255 && eb == 255)){ //Un infinito negativo caso 1
        er = 255;
        sr = 1;
        fr = 0;
        return;
    }
    if((sa == 0 && sb == 1) && (ea == 255 && eb == 255)){ //Un infinito negativo caso 2
        er = 255;
        sr = 1;
        fr = 0;
        return;
    }
    if((sa == 1 && sb == 1) && (ea == 255 && eb == 255)){ //Dos infinitos negativos
        er = 255;
        sr = 0;
        fr = 0;
        return;
    }
    if((sa == 0 && ea == 255) || (sb == 0 && eb == 255)){ //Numero por infinito positivo
        er = 255;
        sr = 0;
        fr = 0;
        return;
    }
    if((sa == 1 && ea == 255) || (sb == 1 && eb == 255)){ //Numero por infinito negativo
        er = 255;
        sr = 1;
        fr = 0;
        return;
    }

    /* ////////////////// PASO 1 ////////////////// */

    unsigned int n = 24;
    sr = sa | sb;
    if(sa == sb){
        sr = 0;
    }

    /* ////////////////// PASO 2 ////////////////// */

    er = ea + eb;

    /* ////////////////// PASO 3 ////////////////// */

    unsigned int P = multiplicacionBinariaSinSigno(ma,mb);
    unsigned int P2 = P;
    P2 = P2 >> n-1;
    if((P2 & 0x01)==0){
        P = P << 1;
    }
    else{
        er += 1;
    }
    unsigned int ma3 = ma;
    ma3 = ma3 >> n-1;
    unsigned int r = (ma3 & 0x01); // o 1 menos
    unsigned int st = 0;
    unsigned int ma2 = ma;
    for(int i=0;i<n-2;i++){
        if((ma2 & 0x01)==1){
            st = 1;
            break;
        }
        ma2 = ma2 >> 1;
    }
    if((r==1 && st==1) || (r==1 && st==0 && ((P & 0x01)==1))){
        P += 1;
    }

    /* ////////////////// DESBORDAMIENTOS ////////////////// */

    if(er>255){
        //Hay overflow
    }

    if(er-126){ // 0 = exponente minimo representable?
        unsigned int t = -126-er;
        if(t>=n){
            //Hay underflow
        }
        else{
            P = P >> t;
            er = -126;
        }
    }

    /* ////////////////// DENORMALES ////////////////// */

    if(ea==0 || eb==0){
        if(er<-126){
            //Igual que Underflow
            unsigned int t=-126-er;
            if(t>=n){
                //hay underflow
            }
            else{
                P = P >> t;
                er=-126;
            }
        }
        if(er>0){
            unsigned int t1=er-126;
            unsigned int k=0;
            unsigned int P2 = P;
            for(int i=23;i>=0;i--){  //Normalizar la mantisa
                if((P2 >> i & 0x01) == 0){
                    k++;
                }
                else{
                    break;
                }
            }
            unsigned int t2=k;
            unsigned int t;
            if(t1<t2){
                t=t1;
            }
            else{
                t=t2;
            }
            er=er-t;
            P = P << t;
        }
        else{
            //Denormal
        }
    }
    er-=127;
    this->fr=P-0x800000;

}

void Alu::divide(){


    /* COMPROBACION CASOS ESPECIALES */

    if((ea == 255 && sa == 0 && fa != 0) || (eb == 255 && sb == 0 && fb != 0)){ // Nan/inf || inf/nan
        sr = 0;
        er = 255;
        fr = 8388607;
        return;
    }

    if((ea == 255 && sa == 0) && (eb == 255 && sb == 0)){ //Inf/inf
        sr = 0;
        er = 255;
        fr = 8388607;
        return;
    }
    if((ea == 255 && sa == 0) && (eb == 255 && sb == 1)){ //Inf/-inf
        sr = 1;
        er = 0;
        fr = 0;
        return;
    }
    if((ea == 255 && sa == 0) && (eb == 0 && sb == 0)){ //Inf/0
        sr = 0;
        er = 255;
        fr = 8388607;
        return;
    }
    if((ea == 0 && sa == 0) && (eb == 255 && sb == 0)){ //0/inf
        sr = 0;
        er = 0;
        fr = 0;
        return;
    }

    this->ea = this->ea - this->eb + 127;
    this->eb = 127;
    if(sa == 0 && sb == 1){
        sa = 1;
        sb = 0;
    }
    if(sa == 1 && sb == 1){
        sa = 0;
        sb = 0;
    }
    union valores valor1;
    union valores valor2;
    valor1.bits.e = ea;
    valor1.bits.f = fa;
    valor1.bits.s = sa;
    valor2.bits.e = eb;
    valor2.bits.f = fb;
    valor2.bits.s = sb;
    float valorB = valor2.real;
    float inversoB = 0;
    if(valorB<0){
        valorB*=-1;
    }
    if((valorB-1.00) < (valorB-1.25)){
        inversoB = 1;
    }
    else{
        inversoB = 0.8;
    }
    union valores valorInverso;
    valorInverso.real = inversoB;
    Alu alu1(valor1.bits.s,valor1.bits.e,valor1.bits.f,valorInverso.bits.s,valorInverso.bits.e,valorInverso.bits.f);
    alu1.multiplica();
    union valores valorRes;
    valorRes.bits.e=alu1.getExponente();
    valorRes.bits.f=alu1.getFraccionaria();
    valorRes.bits.s=alu1.getSigno();
    float x0 =  valorRes.real;
    Alu alu2(valor2.bits.s,valor2.bits.e,valor2.bits.f,valorInverso.bits.s,valorInverso.bits.e,valorInverso.bits.f);
    alu2.multiplica();
    valorRes.bits.e=alu2.getExponente();
    valorRes.bits.f=alu2.getFraccionaria();
    valorRes.bits.s=alu2.getSigno();
    float y0 = valorRes.real;

    union valores v1;
    v1.real = 2;
    union valores v2;
    v2.real = y0;
    union valores v3;
    v3.real = x0;
    float resultado1 = x0;
    float resultado2 = 0;
    int i=0;
    do{
        v2.real = -v2.real;
        Alu alu(v1.bits.s,v1.bits.e,v1.bits.f,v2.bits.s,v2.bits.e,v2.bits.f);
        alu.suma();
        valorRes.bits.e=alu.getExponente();
        valorRes.bits.f=alu.getFraccionaria();
        valorRes.bits.s=alu.getSigno();
        union valores vr;
        vr.real = valorRes.real;

        v2.real = -v2.real;
        Alu alu1(v2.bits.s,v2.bits.e,v2.bits.f,vr.bits.s,vr.bits.e,vr.bits.f);
        alu1.multiplica();
        valorRes.bits.e=alu1.getExponente();
        valorRes.bits.f=alu1.getFraccionaria();
        valorRes.bits.s=alu1.getSigno();
        v2.real = valorRes.real;

        Alu alu2(v3.bits.s,v3.bits.e,v3.bits.f,vr.bits.s,vr.bits.e,vr.bits.f);
        alu2.multiplica();
        valorRes.bits.e=alu2.getExponente();
        valorRes.bits.f=alu2.getFraccionaria();
        valorRes.bits.s=alu2.getSigno();
        v3.real = valorRes.real;
        if(i==0){
            resultado1 = v3.real;
        }
        else if(i==1){
            resultado2 = v3.real;
        }
        else{
            resultado1 = resultado2;
            resultado2 = v3.real;
        }
        i++;
    }while (((resultado2 - resultado1) >= 0.0001) || i < 2);

    sr = v3.bits.s;
    fr = v3.bits.f;
    er = v3.bits.e;

}
