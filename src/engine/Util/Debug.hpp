//
// Created by luiz0tavio on 8/30/18.
//

#ifndef VOLKANO_DEBUG_HPP
#define VOLKANO_DEBUG_HPP

#include <iostream>
#include <cassert>
#include <sstream>
#include <chrono>
#include <vulkan/vulkan.hpp>

//#define PRINT_TIME_SPENT

namespace Engine
{
    #ifdef DEBUG

        #define TRY_CATCH_BLOCK_FN(fn)                                                                              \
            try {                                                                                                   \
                fn;                                                                                    		        \
            } catch(std::system_error &e) {                                                                         \
                std::stringstream error_message;                                                                    \
                error_message << "Something went wrong."<< std::endl;                                               \
                error_message << "Error code: " << e.code() << std::endl;                                           \
                error_message << "Error message: " << e.what() << std::endl;                                        \
                Engine::Debug::logError(error_message.str());                                                       \
                exit(-1);                                                                                           \
            } catch(...) {                                                                                          \
                Engine::Debug::logError("Unknown error.");                                                          \
            }

        #ifdef PRINT_TIME_SPENT
            #define DEBUG_CALL(fn)                                                             		                    \
            {                                                                                               		    \
                auto start_cf = std::chrono::high_resolution_clock::now();                                  		    \
                TRY_CATCH_BLOCK_FN(fn)                                                                                  \
                auto end_cf = std::chrono::high_resolution_clock::now();                                    		    \
                auto duration_cf = std::chrono::duration<double, std::milli>(end_cf - start_cf).count();    		    \
                Engine::Debug::logInfo("Took " + std::to_string(duration_cf) + "ms to execute " + std::string(#fn));    \
            }
        #else
            #define DEBUG_CALL(fn) { TRY_CATCH_BLOCK_FN(fn) }
        #endif

    #else
        #define DEBUG_CALL(fn) fn;
    #endif

    class Debug
    {

    private:

        static vk::DebugUtilsMessengerEXT dbg_utils_messenger;

    public:

        Debug() = delete;

        static void init();
        static void destroy();

        static inline void logInfo(const std::string &log_msg)
        {
        #ifdef DEBUG
            std::cout << log_msg << std::endl;
        #endif
        };

        static inline void logError(const std::string &log_msg)
        {
        #ifdef DEBUG
            std::cerr << log_msg << std::endl;
            exit(-1);
        #endif
        };
    };
}

#endif //VOLKANO_DEBUG_HPP
