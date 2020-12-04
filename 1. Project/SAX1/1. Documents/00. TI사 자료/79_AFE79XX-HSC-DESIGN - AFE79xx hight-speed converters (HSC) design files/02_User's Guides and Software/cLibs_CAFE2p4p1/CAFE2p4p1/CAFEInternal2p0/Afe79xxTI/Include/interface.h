#ifndef ftdi_C_CONNECTOR_H 
#define ftdi_C_CONNECTOR_H 

#ifdef __cplusplus
extern "C" {
#endif

int ftdi_open(char *name);
int ftdi_readReg(int addr);
int ftdi_writeReg(int addr, int val);
int ftdi_close();

#ifdef __cplusplus
}
#endif


#endif