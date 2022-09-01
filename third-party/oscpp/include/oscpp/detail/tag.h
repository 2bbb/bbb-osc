//
//  tag.h
//
//  Created by ISHII 2bit on 2016/03/20.
//
//

#pragma once

#include <string>
#include <iostream>

// cite: https://github.com/openframeworks/openFrameworks/blob/0.9.3/addons/ofxOsc/src/ofxOscMessage.h#L70-L87
/*
	OSC 1.1 specifications types:
 i - 32bit integer
 h - 64bit integer
 f - 32bit floating point number
 d - 64bit (double) floating point number
 s - string
 S - symbol
 c - char
 m - 4 byte midi packet (8 digits hexadecimal)
 T - TRUE (no value required)
 F - FALSE (no value required)
 N - NIL (no value required)
 I - IMPULSE, act as a trigger (no value required), previously named INFINITUM
 t - TIMETAG, an OSC timetag in NTP format, encoded in the data section
	See : http://cnmat.berkeley.edu/system/files/attachments/Nime09OSCfinal.pdf
	*/

namespace OSCPP {
    enum class Tag : char {
        Int32 = 'i',
        Int64 = 'h',
        Float = 'f',
        Double = 'd',
        String = 's',
        Symbol = 'S',
        Char = 'c',
        Midi4 = 'm',
        True = 'T',
        False = 'F',
        NIL = 'N',
        IMPULSE = 'I',
        Timetag = 't',
    };

    inline namespace {
        inline static std::ostream &operator<<(std::ostream & os, Tag tag) {
            return os << static_cast<char>(tag);
        }
    };

    using TagType = char;

#define TagCase(Name) case Tag::Name: return #Name;
    static std::string TagName(TagType tag) {
        switch(static_cast<Tag>(tag)) {
            TagCase(Int32);
            TagCase(Int64);
            TagCase(Float);
            TagCase(Double);
            TagCase(String);
            TagCase(Symbol);
            TagCase(Char);
            TagCase(Midi4);
            TagCase(True);
            TagCase(False);
            TagCase(NIL);
            TagCase(IMPULSE);
            TagCase(Timetag);
            default: return "unknown tag";
        }
    }
#undef TagCase

    static std::string TagName(Tag tag) {
        return TagName(static_cast<TagType>(tag));
    }

};
