# Nasal demons

Each of the following snippets of C code is wrong in some way. Fix it and explain why:

## One
```
    #define MIN(a,b) (((a) < (b)) ? (a) : (b))
    
    P.S. Brackets have been missed inside the macros. As a result a lot of funny effects would be possible due to 
         undefined behavior. E.g. uint8_t variavble = MIN (a ++, b).
    
    Another way is to use inline functions which is doing the same but more comfortable to debug, since macroses 
    will have been processed on preprocessing stage and its debug will be quite challenged. 
    
    So, it might be defined i.e. for ARMCC:    
    __inline uint32_t min (uint32_t a, uint32_t b)
    {
        return ((a <b)? a: b);
    }
    
    For sure, the routine usage has some limitation too and usage e.g. min (a ++, a ++) does not look as a good idea. 
    But it's bit different story :)
```

## Two
```
    int xto3(volatile int *x){
        return *x * *x * *x;
    }
    
    1. The routine has to be changed if there is required to get the result of x * x * x calculation only. 
       For this case value of X variable will not be changed in case of any asynchronous event, 
       e.g. interrupts, reading value of any CPU port directly, etc. because an argument X will be passed by value 
       to the function.
       
        int xto3(volatile int x)
        {
            return (x * x * x);
        }
        
     2. The original routine has some risk to get wrong result for some cases. E.g. read ADC data register directly or 
        similar case. In this case the value of X variable might be changed because ADC finished the conversion 
        cycle between dereferencing the variable X and its multiplication.
```

## Three

```
    int *align_to_bits(int *addr, int bits){
        return ((uintptr_t)addr) & (uintptr_t)((1<<bits)-1);
    }
    
    1. The original routine will return only last bits of value instead of address alignment. Even in that case 
       there is several error. It should be  re-written on this way:
            int *align_to_bits(int *addr, int bits)
            {
                // ULL has to be used to prevent overflow the result e.g bits == 32 and more
                return (int *) ((uintptr_t)addr) & (uintptr_t)((1ULL<<bits)-1ULL); 
            }
            
    2. To align address for the requsted boundary the routine should be re-written:
            int *align_to_bits(int *addr, int bits)
            {
                // The common rule is (Unaligned + bound -1) & ~(bound-1)
                return (int *)(((uintptr_t) addr + ( (uintptr_t) ((1ULL << bits) - 1ULL))) & ~((1ULL << bits) - 1));
            }
```

## Four
```
    int return0(int a, int *b) {
        *b=0;
        *a=42;
        return *b;
    }
    
    There are several errors. Below I provided the list of them:
    1. Compilation error will be generated since dereferencing the variable a, which is not a pointer, is happened.
    2. It is not clear how the routine should be used: 
            a.) if the routine's purpose is to return zero only, it should be re-written like:
                    int return0(void)
                    {
                        return 0;
                    }
                    
            b.) If the purpose routine to set pointer's value to zero and return zero it has to be written like:
                    // volatile int *b should be used if b value might be changed asynchrously, 
                    // e.g. inside interrupt routine.
                    int return0(int *b)
                    {
                        *b = 0;
                        return *b;
                    }
                    
             c.) The routine has to be re-written if both parameter are important and variable a has to be set 
                 for some reson to 42 e.g. for the further processing, to prevent compilation error:
                    int return0(int *a, int *b)
                    {
                        *b=0;
                        *a=42;
                        
                        return *b;
                    }
    
```
