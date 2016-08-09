#pragma once

/*=============================================================================
    Boost.Wave: A Standard compliant C++ preprocessor library
    Sample demonstrating the usage of advanced preprocessor hooks.
    
    http://www.boost.org/

    Copyright (c) 2001-2012 Hartmut Kaiser. Distributed under the Boost 
    Software License, Version 1.0. (See accompanying file 
    LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
=============================================================================*/
#pragma warning( push )
#pragma warning( disable: 4100 4245 4503 4512 4706 4996 )
#include <iostream>
#include <string>
#include <vector>
#include <initializer_list>

///////////////////////////////////////////////////////////////////////////////
//  Include Wave itself
#include <boost/wave.hpp>

///////////////////////////////////////////////////////////////////////////////
// Include the lexer stuff
#include <boost/wave/cpplexer/cpp_lex_token.hpp>    // token class
#include <boost/wave/cpplexer/cpp_lex_iterator.hpp> // lexer class

#pragma warning( pop )

#include "custom_directives.hpp"

//0にすると#lineを出力しない.
//#lineが無いと#includeしているソースコードでエラーが起きても,
//エラーメッセージの行とファイル内での行が一致しない.
#ifndef TOFU_OPENGL_GLSL_PREPROCESSOR_ENABLE_EMIT_LINE_DIRECTIVES
#define TOFU_OPENGL_GLSL_PREPROCESSOR_ENABLE_EMIT_LINE_DIRECTIVES 0
#endif

namespace tofu
{
namespace glsl
{

///////////////////////////////////////////////////////////////////////////////
//  Main entry point
//
//  This sample shows how to use the advanced hooks to output not only the 
//  preprocessed tokens but also the conditional directives found in the input
//  file (these are  commented out, tough) and the tokens from inside the 
//  conditional block which were not evaluated because the corresponding 
//  condition was false. These tokens are commented out as well. 
//
inline std::string glsl_preprocessor(const std::string& source, std::initializer_list<std::string> include_paths)
{
	std::ostringstream out;
	std::string instring = source;

// current file position is saved for exception handling
boost::wave::util::file_position_type current_position;

    try {
    //  The template boost::wave::cpplexer::lex_token<> is the token type to be 
    //  used by the Wave library.
        typedef boost::wave::cpplexer::lex_token<> token_type;

    //  The template boost::wave::cpplexer::lex_iterator<> is the lexer type to
    //  be used by the Wave library.
        typedef boost::wave::cpplexer::lex_iterator<token_type> lex_iterator_type;

    //  This is the resulting context type to use. The first template parameter
    //  should match the iterator type to be used during construction of the
    //  corresponding context object (see below).
        typedef boost::wave::context<std::string::iterator, lex_iterator_type,
                boost::wave::iteration_context_policies::load_file_to_string,
                custom_directives_hooks
            > context_type;

    // The preprocessor iterator shouldn't be constructed directly. It is 
    // to be generated through a wave::context<> object. This wave:context<> 
    // object additionally may be used to initialize and define different 
    // parameters of the actual preprocessing (not done here).
    //
    // The preprocessing of the input stream is done on the fly behind the 
    // scenes during iteration over the context_type::iterator_type stream.
    context_type ctx (instring.begin(), instring.end()/*, argv[1]*/);

    ctx.set_language(boost::wave::enable_long_long(ctx.get_language()));
    ctx.set_language(boost::wave::enable_preserve_comments(ctx.get_language()));
    ctx.set_language(boost::wave::enable_prefer_pp_numbers(ctx.get_language()));
#if TOFU_OPENGL_GLSL_PREPROCESSOR_ENABLE_EMIT_LINE_DIRECTIVES
#else
	ctx.set_language(boost::wave::enable_emit_line_directives(ctx.get_language(), false));
#endif
    for(const auto& inc : include_paths) {
        ctx.add_include_path(inc.c_str());
    }

    // analyze the input file, print out the preprocessed tokens
    context_type::iterator_type first = ctx.begin();
    context_type::iterator_type last = ctx.end();

        while (first != last) {
            current_position = (*first).get_position();
            out << (*first).get_value();
            ++first;
        }
    }
    catch (boost::wave::cpp_exception const& e) {
    // some preprocessing error
        std::cerr 
            << e.file_name() << "(" << e.line_no() << "): "
            << e.description() << std::endl;
    }
    catch (std::exception const& e) {
    // use last recognized token to retrieve the error position
        std::cerr 
            << current_position.get_file() 
            << "(" << current_position.get_line() << "): "
            << "exception caught: " << e.what()
            << std::endl;
    }
    catch (...) {
    // use last recognized token to retrieve the error position
        std::cerr 
            << current_position.get_file() 
            << "(" << current_position.get_line() << "): "
            << "unexpected exception caught." << std::endl;
    }

    return out.str();
}
}
}

