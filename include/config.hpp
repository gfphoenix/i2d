/*
 * config.h
 *
 *  Created on: Apr 24, 2008
 *      Author: wuhao
 */

#ifndef CONFIG_H_
#define CONFIG_H_
#define DEL_COPY(Tp)    Tp(const Tp&)=delete; \
                        Tp & operator=(const Tp &)=delete

#define FORCE_STACK_CLASS  \
            static void *operator new(std::size_t n)=delete;    \
            static void *operator new[](std::size_t n)=delete;  \
            static void operator delete(void *ptr)=delete;      \
            static void operator delete[](void *ptr)=delete;

#define UNUSED(xx)  do{}while(!sizeof(xx))
#define BEGIN_NS(ns)  namespace ns \
                       {
#define END_NS(ns)  }

#include <debug>
#include <gl>

#endif /* CONFIG_H_ */
