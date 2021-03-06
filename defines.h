/*
 *  defines.h
 *  PandoraControls
 *
 *  Created by Public Nuisance on 7/29/09.
 *  Copyright 2009 __MyCompanyName__. All rights reserved.
 *
 */


#ifdef TESTING
	#define NSLine() NSLog(@"%s %s %d", __FILE__, __FUNCTION__, __LINE__)
	#define HookLog(); \
		{ \
		uint32_t bt=0; \
		__asm__("mov %0, lr": "=r"(bt)); \
		NSLog(@"[%@ %s] bt=%x", [[self class] description], sel_getName(sel), bt); \
		}
	#define SelLog(); \
		{ \
		uint32_t bt=0; \
		__asm__("mov %0, lr": "=r"(bt)); \
		NSLog(@"%s bt=%x", __FUNCTION__, bt); \
		}
	#define NSType(obj) \
		NSLog(@"%@* " #obj  ";", [[obj class] description])
	#define NSDesc(obj) \
		NSLog(@"%@* " #obj  ";", [obj description])
	#define NSRect(obj) \
		NSLog(@ #obj " = {{%f %f}{%f %f}}", obj.origin.x, obj.origin.y, obj.size.width, obj.size.height)
	#define CommonLog(fmt, ...) \
		{ \
			syslog(5, fmt, ##__VA_ARGS__); \
			fprintf(stderr, fmt "\n", ##__VA_ARGS__); \
		}

	#define TRACE() \
	{ \
		NSArray* callStackReturnAddresses = [NSThread callStackReturnAddresses];	\
		int ncallStackReturnAddresses = [callStackReturnAddresses count];	\
		for(int level =0; level < ncallStackReturnAddresses; level++) \
		{	\
			NSNumber* addr = [callStackReturnAddresses objectAtIndex: level];	\
			uint32_t bt = [addr unsignedIntValue];	\
			Dl_info info; \
			dladdr((void*)bt, &info); \
			char* fname = strrchr(info.dli_fname, '/'); \
			if(fname) \
				fname++;\
			if(info.dli_sname) \
			{ \
				CommonLog("%d: %s %08x (%s + %08x)", level, fname, (uint32_t)bt - (uint32_t)info.dli_fbase, info.dli_sname, (uint32_t) bt - (uint32_t) info.dli_saddr); \
			} \
			else if(fname)\
			{ \
				CommonLog("%d: %s %08x (unknown)", level, fname, (uint32_t)bt - (uint32_t)info.dli_fbase); \
			} \
			else	\
			{	\
				CommonLog("%d: (unknown) %08x (unknown)", level, (uint32_t)bt); \
			}	\
		}	\
	}
#else
	#define NSLine()
	#define NSLog(...)
	#define HookLog();
	#define SelLog();
	#define NSType(...)
	#define NSDesc(...)
	#define NSRect(...)
	#define CommonLog(...)
	#define TRACE()
#endif

	#define PROFILE(increment_var)	\
		for(timeval tim1 = (struct timeval){0}, tim2 = (struct timeval){0}; \
				!gettimeofday(&tim1, NULL) && !tim2.tv_sec; \
				gettimeofday(&tim2, NULL), increment_var += tim2.tv_usec - tim1.tv_usec + (tim2.tv_sec-tim1.tv_sec)*1000000)
	
	
	#define CommonLog_F(fmt, ...) \
		{ \
			syslog(5, fmt, ##__VA_ARGS__); \
			fprintf(stderr, fmt "\n", ##__VA_ARGS__); \
		}

#if !defined(__arm64__)
	#define TRACE_F() \
	{ \
		NSArray* callStackReturnAddresses = [NSThread callStackReturnAddresses];	\
		int ncallStackReturnAddresses = [callStackReturnAddresses count];	\
		for(int level =0; level < ncallStackReturnAddresses; level++) \
		{	\
			NSNumber* addr = [callStackReturnAddresses objectAtIndex: level];	\
			uint32_t bt = [addr unsignedIntValue];	\
			Dl_info info; \
			dladdr((void*)bt, &info); \
			char* fname = strrchr(info.dli_fname, '/'); \
			if(fname) \
				fname++;\
			if(info.dli_sname) \
			{ \
				CommonLog_F("%d: %s %08x (%s + %08x)", level, fname, (uint32_t)bt - (uint32_t)info.dli_fbase, info.dli_sname, (uint32_t) bt - (uint32_t) info.dli_saddr); \
			} \
			else if(fname)\
			{ \
				CommonLog_F("%d: %s %08x (unknown)", level, fname, (uint32_t)bt - (uint32_t)info.dli_fbase); \
			} \
			else	\
			{	\
				CommonLog_F("%d: (unknown) %08x (unknown)", level, (uint32_t)bt); \
			}	\
		}	\
	}
#else
	#define TRACE_F() \
	{ \
		NSArray* callStackReturnAddresses = [NSThread callStackReturnAddresses];	\
		int ncallStackReturnAddresses = [callStackReturnAddresses count];	\
		for(int level =0; level < ncallStackReturnAddresses; level++) \
		{	\
			NSNumber* addr = [callStackReturnAddresses objectAtIndex: level];	\
			uint64_t bt = [addr unsignedLongValue];	\
			Dl_info info; \
			dladdr((void*)bt, &info); \
			char* fname = strrchr(info.dli_fname, '/'); \
			if(fname) \
				fname++;\
			if(info.dli_sname) \
			{ \
				CommonLog_F("%d: %s %016llx (%s + %016llx)", level, fname, (uint64_t)bt - (uint64_t)info.dli_fbase, info.dli_sname, (uint64_t) bt - (uint64_t) info.dli_saddr); \
			} \
			else if(fname)\
			{ \
				CommonLog_F("%d: %s %016llx (unknown)", level, fname, (uint64_t)bt - (uint64_t)info.dli_fbase); \
			} \
			else	\
			{	\
				CommonLog_F("%d: (unknown) %016llx (unknown)", level, (uint64_t)bt); \
			}	\
		}	\
	}

#endif


#define HOOKDEF(type, class, name, args...) \
static type (*_ ## class ## $ ## name)(class *self, SEL sel, ## args); \
static type $ ## class ## $ ## name(class *self, SEL sel, ## args)

#define HOOK(type, class, name, args...) \
static type $ ## class ## $ ## name(class *self, SEL sel, ## args)

#define CALL_ORIG(class, name, args...) \
_ ## class ## $ ## name(self, sel, ## args)

#define GETCLASS(class) \
Class $ ## class  = objc_getClass(#class)


#define HOOKMESSAGE(class, sel, selnew) \
MSHookMessageEx( $ ## class, @selector(sel), (IMP)$ ## class ## $ ## selnew, (IMP*)&_ ## class ## $ ## selnew);
//_ ## class ## $ ## selnew = MSHookMessage( $ ## class, @selector(sel), &$ ## class ## $ ## selnew);

#define HOOKCLASSMESSAGE(class, sel, selnew) \
MSHookMessageEx( object_getClass($ ## class), @selector(sel), (IMP)$ ## class ## $ ## selnew, (IMP*)&_ ## class ## $ ## selnew);
//_ ## class ## $ ## selnew = MSHookMessage( object_getClass($ ## class), @selector(sel), &$ ## class ## $ ## selnew);

#define IVGETVAR(type, name); \
static type name; \
Ivar IV$ ## name = object_getInstanceVariable(self, #name, reinterpret_cast<void **> (& name));

#define GETVAR(type, name); \
static type name; \
object_getInstanceVariable(self, #name, reinterpret_cast<void **> (& name));

#define GETIVAR(type, name) \
name = (type) object_getIvar(self, IV$ ## name)

#define SETIVAR(name) \
object_setIvar(self, IV$ ## name, (id) name)

#define SETVAL(name, value); \
name = value; \
object_setIvar(self, IV$ ## name, (id) name);

#define CLASSALLOC(class) \
[objc_getClass(#class) alloc]



#ifndef NO_PROTECTION
	#define FULL_PROTECTION
#endif
