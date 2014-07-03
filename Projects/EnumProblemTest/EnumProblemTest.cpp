/*
 * EnumProblemTest.cpp
 *
 * Created: 6/17/2014 7:44:51 PM
 *  Author: Matt
 */ 

#include <avr/io.h>
#include <avr/wdt.h>


struct  asmStruct{
	unsigned int thisPointer;
	unsigned int pgmMemPointer;
};

/*asm ("LDI %[thisReg1], %[thisPtr1]"
	 "LDI %[thisReg2], %[thisPtr2]"
	 "RCALL PC+%[funcAddr]" )*/
class A
{
public:
	A( unsigned char eorValue )	
	: m_MemberOutputTest( eorValue )
	{}
		
	void doSomething()
	{
		asm volatile ( "callable:"::);
		PORTB ^= m_MemberOutputTest;
	}
protected:
private:
	unsigned char m_MemberOutputTest;
};

A doofuss(0x0F) ;
A foosuss(0xF0) ;

int main(void)
{
	wdt_disable();
	
	while(1){		
		doofuss.doSomething();
		foosuss.doSomething();
		asm volatile (  "LDI R24,0x60\n\t"
						"LDI R25,0x00\n\t"
						"RCALL callable"
						::
						);
	}
}

