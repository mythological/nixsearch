#ifndef __ST4X__
#define __ST4X__

struct St4x;
typedef struct St4x St4x;

int STXPop(St4x *s);
void STXDestroy(St4x *s);
void STXPrint(St4x *s);
int  STXPush(int c, St4x *s);
int  STXLen(St4x *s);
St4x *STXSpawn();

#endif
