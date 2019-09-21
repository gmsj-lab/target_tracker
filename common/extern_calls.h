#ifndef _EXTERN_CALLS_H_
#define _EXTERN_CALLS_H_

#ifdef __cplusplus
 extern "C" {
#endif

extern void setupSoftwareReprogramming ( void ) ;
extern void performSoftwareReboot      ( void ) ;
extern void softwareReprogrammingReq   ( void ) ;
extern void softwareRebootReq		   ( void ) ;
/*
extern void app_error		    	   ( void ) ;
extern void delay 					   ( long ) ;
*/

//#define MAIN_ERROR

#ifdef __cplusplus
}
#endif

#endif /* _EXTERN_CALLS_H_ */
