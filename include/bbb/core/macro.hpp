//
//  bbb/core/macro.hpp
//
//  Created by 2bit on 2022/08/22.
//

#ifndef bbb_core_macro_hpp
#define bbb_core_macro_hpp

#ifndef __has_feature
#   define __has_feature(x) 0
#endif

#ifndef __has_extension
#   define __has_extension __has_feature
#endif

#ifndef __has_cpp_attribute
#   define __has_cpp_attribute(x) 0
#endif

#if __has_cpp_attribute(clang::fallthrough)
#   define BBB_FALLTHROUGH [[clang::fallthrough]]
#else
#   define BBB_FALLTHROUGH
#endif

#ifndef __has_attribute
#   define __has_attribute(x) 0
#endif

#if __has_cpp_attribute(nodiscard)
#   define BBB_NO_DISCARD [[ nodiscard ]]
#else
#   define BBB_NO_DISCARD
#endif

#endif /* bbb_core_macro_hpp */
