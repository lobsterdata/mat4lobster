/*=================================================================
 *
 * Copyright 2013 Ruihong Huang (ruihong.huang@wiwi.hu-berlin.de)
 *
 * Reference:
 *
 *
 *=================================================================*/
#include "mex.h"
#include <math.h>
#include <stdlib.h>
#include <stdio.h>

/* Input Arguments */

#define MESSAGE prhs[0]
#define TIME    prhs[1]

/* Output Arguments */

#define	ORDERFLOW plhs[0]

/* real working c function*/
void writeRecord(double *record, double tim, double market,double size, double price, double bs,
        double hprice, double tradePrice, double tradeSize)
{
    record[0]=tim;
    record[1]=market;
    record[2]=size;
    record[3]=price;
    record[4]=bs;
    record[5]=hprice;
    if (tradePrice >0)
        record[6]=tradePrice*1.0/tradeSize;
    else
        tradePrice =0;
    record[7]=tradeSize;
}

void readMessage(double const * const message, double *time,double *eventcode,double *size,
        double *price, double *bs, double *marketable,size_t r)
{
    *time=message[r*6];
    *eventcode=message[r*6+1];
    *size=message[r*6+3];
    *price=message[r*6+4];
    *bs=message[r*6+5];
    *marketable = (*eventcode==4 || *eventcode==5);
    if (marketable)
        *bs = -*bs;
}

void toOrderFlow(double * const orderflow, double const * const message,
        size_t rows, double tim)
{
    double *of=orderflow;
    size_t r;
    double time=0.0,marketable=0.0,size=0.0,price=0.0,bs=0.0;
    double hprice=0,tradePrice=0,tradeSize = 0, eventcode=0,pre_eventcode =0;
    double ordertime=0,ordersize = 0, orderprice=0, orderbs=0,
            ordermarketable=0;
    
    readMessage(message,&ordertime,&pre_eventcode,&ordersize,&orderprice,
            &orderbs,&ordermarketable,0);
    for (r=1; r<rows; r++){
        readMessage(message,&time,&eventcode,&size,&price,&bs,&marketable,r);
        /*Higer than time threshold or different types or previous is a new order*/
        if (time-ordertime>tim  || /*time greater than threshold*/
                orderbs!=bs || /*Not same trade direction*/
                pre_eventcode==1 || /* preceding is already a limit order */
                (eventcode ==1 && bs<0 && price<orderprice) ||
                /* new sell limit order with price less than preceding sell order*/
                (eventcode ==1 && bs>0 && price>orderprice))
            /* new buy limit orer with price higher than preceding buy order*/
        {
            writeRecord(of,ordertime,marketable,ordersize,orderprice,orderbs,hprice,tradePrice,tradeSize); /*Write previous record*/
            of +=8;
            hprice = 0;
            ordersize = 0;
            ordertime = 0;
            orderprice = 0;
            orderbs = bs;
            tradePrice = tradeSize = 0;
            ordermarketable =marketable;
        }
        pre_eventcode = eventcode;
        ordersize += size;
        orderprice = price;
        ordertime = time;
        if (marketable){
            tradeSize += size;
            tradePrice += size*price;
            if (eventcode >4)
                hprice=price;
        }
    }
    writeRecord(of,ordertime,marketable,ordersize,orderprice,orderbs,hprice,
            tradePrice,tradeSize); /*write the last record*/
}

/* mex function to matlab*/
void mexFunction( int nlhs, mxArray *plhs[],
        int nrhs, const mxArray*prhs[] )
{
    double *orderflow, *tim;
    double *message;
    size_t n = mxGetN(MESSAGE);
    ORDERFLOW =  mxCreateNumericMatrix(8,n, mxDOUBLE_CLASS,mxREAL);
    
    message = mxGetPr(MESSAGE);
    tim =mxGetPr(TIME);
    orderflow = mxGetPr(ORDERFLOW);
    
    toOrderFlow(orderflow,message,n,*tim);
    return;
}

