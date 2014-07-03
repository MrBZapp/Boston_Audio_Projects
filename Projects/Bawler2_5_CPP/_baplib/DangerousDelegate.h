/* 
* DangerousDelegate.h
*
* Created: 6/18/2014 5:19:22 PM
* Author: Matt
*/


#ifndef __DANGEROUSDELEGATE_H__
#define __DANGEROUSDELEGATE_H__



class DangerousDelegate
{
//functions
public:
	virtual ~DangerousDelegate(){}
	virtual void Method1() = 0;
	virtual void Method2() = 0;

}; //DangerousDelegate

#endif //__DANGEROUSDELEGATE_H__
