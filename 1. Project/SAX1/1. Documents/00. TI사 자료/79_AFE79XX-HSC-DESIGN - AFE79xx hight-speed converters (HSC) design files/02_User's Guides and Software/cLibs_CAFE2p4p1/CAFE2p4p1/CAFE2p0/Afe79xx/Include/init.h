

#ifndef INIT_H
#define INIT_H

int8_t configAfeFromFileFormat0(uint8_t afeId, char *file, uint8_t breakAtPollFail, uint8_t breakAtReadCheckFail);
int8_t configAfeFromFileFormat5(uint8_t afeId, char *file, uint8_t breakAtPollFail, uint8_t breakAtReadCheckFail);
int8_t configAfeFromFile(uint8_t afeId, uint8_t logFormat, char *file, uint8_t breakAtPollFail, uint8_t breakAtReadCheckFail);

#endif
