#pragma once

#pragma warning( push )
#pragma warning( disable: 4100 4245 4503 4512 4996 )

#include <cstdio>
#include <ostream>
#include <string>
#include <algorithm>

#include <boost/assert.hpp>
#include <boost/config.hpp>

#include <boost/wave/token_ids.hpp>
#include <boost/wave/util/macro_helpers.hpp>
#include <boost/wave/preprocessing_hooks.hpp>

#pragma warning(pop)

namespace tofu
{
namespace glsl
{

///////////////////////////////////////////////////////////////////////////////
//  
//  The custom_directives_hooks policy class is used to register some
//  of the more advanced (and probably more rarely used hooks with the Wave
//  library.
//
//  This policy type is used as a template parameter to the boost::wave::context<>
//  object.
//
///////////////////////////////////////////////////////////////////////////////
class custom_directives_hooks
:   public boost::wave::context_policies::default_preprocessing_hooks
{
public:
    ///////////////////////////////////////////////////////////////////////////
    //
    //  The function 'found_unknown_directive' is called, whenever an unknown 
    //  preprocessor directive was encountered.
    //
    //  The parameter 'ctx' is a reference to the context object used for 
    //  instantiating the preprocessing iterators by the user.
    //
    //  The parameter 'line' holds the tokens of the entire source line
    //  containing the unknown directive.
    //
    //  The parameter 'pending' may be used to push tokens back into the input 
    //  stream, which are to be used as the replacement text for the whole 
    //  line containing the unknown directive.
    //
    //  The return value defines, whether the given expression has been 
    //  properly interpreted by the hook function or not. If this function 
    //  returns 'false', the library will raise an 'ill_formed_directive' 
    //  preprocess_exception. Otherwise the tokens pushed back into 'pending'
    //  are passed on to the user program.
    //
    ///////////////////////////////////////////////////////////////////////////
    template <typename ContextT, typename ContainerT>
    bool
    found_unknown_directive(ContextT const& /*ctx*/, ContainerT const& line, 
        ContainerT& pending)
    {
        namespace wave = boost::wave;

        typedef typename ContainerT::const_iterator iterator_type;
        iterator_type it = line.begin();
        wave::token_id id = wave::util::impl::skip_whitespace(it, line.end());

        if (id != wave::T_IDENTIFIER)
            return false;       // nothing we could do

        if ((*it).get_value() == "version" || (*it).get_value() == "extension")
        {
            // handle #version and #extension directives
            std::copy(line.begin(), line.end(), std::back_inserter(pending));
            return true;
        }

        return false;           // unknown directive
    }
};

}
}

