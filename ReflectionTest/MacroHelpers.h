
//Can call STR/CAT in a macro, and have the arguement stringified/concatenated before evaluation by the rest of the macro

#define _STR(x) #x
#define STR(x) _STR(x)

#define _CAT(x,y) x##y
#define CAT(x,y) _CAT(x,y)


#define NAME_GENERATOR_INTERNAL( _ ) CAT( GENERATED_NAME, _ )
#define NAME_GENERATOR( ) NAME_GENERATOR_INTERNAL( __COUNTER__ )