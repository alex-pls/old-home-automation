/**
 * @file    test_WindowsMobile.cpp
 * @brief   Test classes: wosh::devices::WindowsMobile
 *
 * @version $Id: test_WindowsMobile.cpp 435 2009-08-21 02:15:11Z alex $
 * @author  Alessandro Polo
 ****************************************************************************/
/* Copyright (c) 2007-2009, WOSH - Wide Open Smart Home 
 * by Alessandro Polo - OpenSmartHome.com
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *     * Neither the name of the OpenSmartHome.com WOSH nor the
 *       names of its contributors may be used to endorse or promote products
 *       derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY Alessandro Polo ''AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL Alessandro Polo BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 ******************************************************************************/

 #include <woshDefs.h>
 #include <core/Logger.h>
 #include "WindowsMobile.h"

using namespace std;
using namespace wosh;
using namespace wosh::devices;


void test_class_WindowsMobile();




int test_WindowsMobile()
 {
	Log::log( "test_WindowsMobile() : Starting TEST" );

	test_class_WindowsMobile();


	Log::log( "test_WindowsMobile() : Finished TEST" );
	return 0;
 }




void test_class_WindowsMobile()
 {
	Log::log( " test_class_WindowsMobile() : Starting TEST for class 'WindowsMobile'" );

	WindowsMobile dsl;

	Log::log( " test_class_WindowsMobile() : Finished TEST for class 'WindowsMobile'" );
 }



int main()
 {
	return test_WindowsMobile();
 }
