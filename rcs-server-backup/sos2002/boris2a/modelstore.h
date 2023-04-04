// $Header$

// $Log$
// Revision 1.1  2000-02-18 10:30:40+00  jjs
// Vertex Buffer version
//

//================================================================================================================================
// modelStore.h
// --------------
//
//		Holding of all the models in the system. Each model filename stored for the appropriate index,
//================================================================================================================================

#ifndef _BS_modelStore
#define _BS_modelStore




void startModelStore();
model * getModelPtr( char * filename, float modelscale,  bool uniqueCopy, char * SSname, bool useVB = false );
void removeModelStore();


#endif	// _BS_textureStore

//================================================================================================================================
//END OF FILE
//================================================================================================================================
