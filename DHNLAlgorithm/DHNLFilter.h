#ifndef DHNL_DHNLFILTER_H
#define DHNL_DHNLFILTER_H


//algorithm wrapper
#include "xAODAnaHelpers/Algorithm.h"



class DHNLFilter : public xAH::Algorithm
{
public:
    // this is a standard constructor
    DHNLFilter ();

    // these are the functions inherited from Algorithm
    virtual EL::StatusCode setupJob (EL::Job& job);
    virtual EL::StatusCode fileExecute ();
    virtual EL::StatusCode histInitialize ();
    virtual EL::StatusCode changeInput (bool firstFile);
    virtual EL::StatusCode initialize ();
    virtual EL::StatusCode execute ();
    virtual EL::StatusCode postExecute ();
    virtual EL::StatusCode finalize ();
    virtual EL::StatusCode histFinalize ();

    // these are the functions not inherited from Algorithm
    virtual EL::StatusCode configure ();


ClassDef(DHNLFilter, 1);
};

#endif //DHNL_DHNLFILTER_H
