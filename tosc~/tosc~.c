
 

 
 
 #include "ext.h"            // standard Max include, always required (except in Jitter)
 #include "ext_obex.h"        // required for "new" style objects
 #include "z_dsp.h"        // required for MSP objects
 #include "ext_post.h"
 #include <math.h>
 #include  <stdio.h>

 #define PI 3.14159265
 #define BSIZE 128
 #define RATE 44100
 #define TSIZE 64
 #define TWOPI PI * 2

//base template for this external is the simplemsp~ object available in the max-sdk
//please use the version 8.2 for this build


//struct for the oscillator implementation with pointer to processing which will implement the wave_this_osc synth processing
 typedef struct _base_osc_ {
     double h;          //harmonics
     double amp;   //amplitude
     double tab[TSIZE]; //table for the wave synth with a set size.
     double sig[BSIZE];  //array for the signal
     double sr;         //sample rate
     double frq;        //frequency (adjustable via inlet 1)
      double ph;   //phase
     double *(*process)(struct _base_osc_ *, double, double); //processing method allocated with parameter space for tsize and bsize
 } Base_osc;


double *wave_this_osc(Base_osc *syn, double tsize, double bsize) {
    for(int i = 0; i < tsize; i++) {
        int ind = (int)(syn->ph * tsize); //method takes in table, phase, and frequency dereferenced from inputed oscillator via syn
        if (ind < 0) {
            ind += tsize;
        } else if (ind >= tsize) {
            ind -= tsize;
        }
        syn->sig[i] = 0.5 * syn->tab[ind];
        syn->ph += syn->frq / syn->sr;
        while (syn->ph >= 1.0) {   //ensuring wrapping the phase.  Initially had table size but had to resort to 0 to 1.
            syn->ph -= 1.0;
            {
        while (syn->ph < 0) {
            syn->ph += 1.0;
                }
            }
        }
    }
    return syn->sig;
}
    
    
    // struct to represent the object's state
    typedef struct _tosc {
        t_pxobject        ob;            // the object itself (t_pxobject in MSP instead of t_object)
        Base_osc       osc;
        double         *sig;
        double         *t; //for the wave table
        double         tsize;  //table size
        double             bsize;  //buffer size
        double            amp;     // storing the amplitude here
        double              f;  //frequency
        double             h; //harmonics
    } t_tosc;
    
    
    // global class pointer variable
    static t_class *tosc_class = NULL;
    
    // method prototypes here
    void *simplemsp_new(t_symbol *s, long argc, t_atom *argv);
    
    void simplemsp_free(t_tosc *x);
    void simplemsp_assist(t_tosc *x, void *b, long m, long a, char *s);
    void simplemsp_float(t_tosc *x, double f);
    void simplemsp_int(t_tosc *x, long n);
    void simplemsp_dsp64(t_tosc *x, t_object *dsp64, short *count, double samplerate, long maxvectorsize, long flags);
    void simplemsp_perform64(t_tosc *x, t_object *dsp64, double **ins, long numins, double **outs, long numouts, long sampleframes, long flags, void *userparam);
    void tosc_setting_freq(t_symbol *x);
    void tosc_setting_harm(t_symbol *x);

    
    //***********************************************************************************************
    
//main function here
    void ext_main(void *r)
    {
        // object initialization, note the use of dsp_free for the freemethod, which is required
        
        t_class *c = class_new("tosc~", (method)simplemsp_new, (method)dsp_free, (long)sizeof(t_tosc), 0L, A_GIMME, 0);
        
        class_addmethod(c, (method)simplemsp_float,        "float",    A_FLOAT, 0);
        class_addmethod(c, (method)simplemsp_int,  "int", A_LONG, 0);
        class_addmethod(c, (method)simplemsp_dsp64,        "dsp64",    A_CANT, 0);
        class_addmethod(c, (method)simplemsp_assist,    "assist",    A_CANT, 0);
        class_dspinit(c);
        class_register(CLASS_BOX, c);
        tosc_class = c;
    }

    //where main initialization of the t_tosc parameters are.  Including memory allocation
    void *simplemsp_new(t_symbol *s, long argc, t_atom *argv)
    {
        t_tosc *x = (t_tosc *)object_alloc(tosc_class);
        
        if (x) {
            
            dsp_setup((t_pxobject *)x, 2);    // MSP inlets: arg is # of inlets
            outlet_new((t_object *)x, "signal");         // signal outlet (note "signal" rather than NULL)
            
            x->h = 10;  //initializing with basic parameters
            x->osc.h = 10;  //harmonics
            x->tsize = TSIZE;
            x->bsize = BSIZE;
            
            x->osc.ph = 0.0; //setting the inital phase here which can be manipulated with 0.0 to 1.0 input inlet
            x->f = 440.0; //default frequency
            x->osc.frq = 440;
            x->amp = 0.5;
            x->osc.sr = sys_getsr();  //getting sample rate from the system
            
            x->osc.process = wave_this_osc; //process using the wave_synth function
            
            //ability to write in arguments for frequency and harmonics
            if (argc > 0 && atom_gettype(argv) == A_FLOAT) {
                x->osc.frq = atom_gettype(argv);
                x->f = atom_gettype(argv);
            }
            
            if (argc > 1 && atom_gettype(argv + 1) == A_FLOAT) {
                x->osc.h = atom_gettype(argv + 1);
                x->h = atom_gettype(argv + 1);
            }

            
    
            //creating the wavetable here in the initialization
            //Fourier Table with harmonics

            for (int i = 0; i < x->tsize; i++) {
                x->osc.tab[i] = 0.0;
                
                //loop cycles through harmonics to create waveform
                for (int j = 1; j <= x->h; j++) { //j is the harmonic (harm)
                    
                    double time = (double)i/x->osc.sr;
    
                    //checks string input from message inlet and iterates through the harmonics depending on the value,
                    x->osc.tab[i] += sin(TWOPI * x->f * j * time)/j; //if left as "none" then loops through all the harmonics
                }
                    x->osc.tab[i] *= x->amp;
                
            }
            return (x);
        }
        return NULL;  //in case of memory issues
    }
    
    
    // NOT CALLED!, we use dsp_free for a generic free function
    void simplemsp_free(t_tosc *x)
    {
        
        dsp_free((t_pxobject *)x);
    }
    
    
    void simplemsp_assist(t_tosc *x, void *b, long m, long a, char *s)
    {
        if (m == ASSIST_INLET) { //inlet descripter messages when the mouse is placed over the inlets and outlets
            if (a == 0) {
                sprintf(s, "Frequency");
                } else if (a == 1) {
                sprintf(s, "Harmonics");
            } else {
                sprintf(s, "Signal Output");
            }
        }
    }
    
    
    void simplemsp_float(t_tosc *x, double f)
    {
        //post("Received the frequency: %.5f", f);
        
        x->osc.frq = f;
        x->f = f;
    }
    
    //handles in freq input is a int rather than float
    void simplemsp_int(t_tosc *x, long n) {
        
        //x->osc.h = h;
        //x->h = h;
        simplemsp_float(x, (double) n);
    }
    
    
    
    // registers a function for the signal chain in Max
    void simplemsp_dsp64(t_tosc *x, t_object *dsp64, short *count, double samplerate, long maxvectorsize, long flags)
    {
        
        x->osc.sr = samplerate;
        
        object_method(dsp64, gensym("dsp_add64"), x, simplemsp_perform64, 0, NULL);
    }
    
    
    
    // this is the 64-bit perform method audio vectors
    void simplemsp_perform64(t_tosc *x, t_object *dsp64, double **ins, long numins, double **outs, long numouts, long sampleframes, long flags, void *userparam)
    {
        t_double *inF = ins[0];        // frequency
        t_double *inH = ins[1];      //harmonics
        t_double *outL = outs[0];    // we get audio for each outlet of the object from the **outs argument
        
        int n = (int)sampleframes;
        
        // this perform method simply copies the input to the output, offsetting the value
        while (n--) {
            
            double* out_s = x->osc.process(&(x->osc), x->tsize, x->bsize);
            
            for(int i = 0; i < n; i++){
                double f = inF[i]; //putting the frequency in f from the input inlet
                if (f != 0.0) {
                    x->osc.frq = f;
                }
            }
            
            for(int i = 0; i < n; i++){
                double harm = inH[i]; //putting the frequency in f from the input inlet
                if (harm != 0.0) {
                    x->osc.h = harm;
                }
            }
            
            
            double output = (x->amp) * out_s[n % BSIZE]; //amp to scale to output processing
            

            
            if (output > 1.0) {                //making sure no happen beyond clipping beyond -1.0 and 1.0
                output = 1.0;
            } else if (output < -1.0) {
                output = -1.0;
            }
            
            *outL++ = output;  //output to outlet array
            
        }
    }


                     

