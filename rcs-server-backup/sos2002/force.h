
// force feedback interface

void StopEffects();
void Jolt(int i,DWORD intensity,float duration,DWORD direction);
void SetForceFeedback(long left,long right,long offset,float revs);

#define	NUM_EFFECTS	8

#define	EFF_HARD	0
#define	EFF_SOFT	1
#define	EFF_TRIANGLE	2
#define	EFF_SINE	3
#define	EFF_CONDITION	4
