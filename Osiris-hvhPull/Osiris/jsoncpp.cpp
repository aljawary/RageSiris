/// Json-cpp amalgamated source (http://jsoncpp.sourceforge.net/).
/// It is intended to be used with #include "json/json.h"

// //////////////////////////////////////////////////////////////////////
// Beginning of content of file: LICENSE
// //////////////////////////////////////////////////////////////////////

/*
The JsonCpp library's source code, including accompanying documentation,
tests and demonstration applications, are licensed under the following
conditions...

Baptiste Lepilleur and The JsonCpp Authors explicitly disclaim copyright in all
jurisdictions which recognize such a disclaimer. In such jurisdictions,
this software is released into the Public Domain.

In jurisdictions which do not recognize Public Domain property (e.g. Germany as of
2010), this software is Copyright (c) 2007-2010 by Baptiste Lepilleur and
The JsonCpp Authors, and is released under the terms of the MIT License (see below).

In jurisdictions which recognize Public Domain property, the user of this
software may choose to accept it either as 1) Public Domain, 2) under the
conditions of the MIT License (see below), or 3) under the terms of dual
Public Domain/MIT License conditions described here, as they choose.

The MIT License is about as close to Public Domain as a license can get, and is
described in clear, concise terms at:

   http://en.wikipedia.org/wiki/MIT_License

The full text of the MIT License follows:

========================================================================
Copyright (c) 2007-2010 Baptiste Lepilleur and The JsonCpp Authors

Permission is hereby granted, free of charge, to any person
obtaining a copy of this software and associated documentation
files (the "Software"), to deal in the Software without
restriction, including without limitation the rights to use, copy,
modify, merge, publish, distribute, sublicense, and/or sell copies
of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be
included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS
BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
========================================================================
(END LICENSE TEXT)

The MIT license is compatible with both the GPL and commercial
software, affording one all of the rights of Public Domain with the
minor nuisance of being required to keep the above copyright notice
and license text in the source code. Note also that by accepting the
Public Domain "license" you can re-license your copy using whatever
license you like.

*/

// //////////////////////////////////////////////////////////////////////
// End of content of file: LICENSE
// //////////////////////////////////////////////////////////////////////






#include "json/json.h"

#ifndef JSON_IS_AMALGAMATION
#error "Compile with -I PATH_TO_JSON_DIRECTORY"
#endif


// //////////////////////////////////////////////////////////////////////
// Beginning of content of file: src/lib_json/json_tool.h
// //////////////////////////////////////////////////////////////////////

// Copyright 2007-2010 Baptiste Lepilleur and The JsonCpp Authors
// Distributed under MIT license, or public domain if desired and
// recognized in your jurisdiction.
// See file LICENSE for detail or copy at http://jsoncpp.sourceforge.net/LICENSE

#ifndef LIB_JSONCPP_JSON_TOOL_H_INCLUDED
#define LIB_JSONCPP_JSON_TOOL_H_INCLUDED

#if !defined(JSON_IS_AMALGAMATION)
#include <json/config.h>
#endif

// Also support old flag NO_LOCALE_SUPPORT
#ifdef NO_LOCALE_SUPPORT
#define JSONCPP_NO_LOCALE_SUPPORT
#endif

#ifndef JSONCPP_NO_LOCALE_SUPPORT
#include <clocale>
#endif

/* This header provides common string manipulation support, such as UTF-8,
 * portable conversion from/to string...
 *
 * It is an internal header that must not be exposed.
 */

namespace Json {
    static inline char getDecimalPoint() {
#ifdef JSONCPP_NO_LOCALE_SUPPORT
        return '\0';
#else
        struct lconv* lc = localeconv();
        return lc ? *(lc->decimal_point) : '\0';
#endif
    }

    /// Converts a unicode code-point to UTF-8.
    static inline String codePointToUTF8(unsigned int cp) {
        String result;

        // based on description from http://en.wikipedia.org/wiki/UTF-8

        if (cp <= 0x7f) {
            result.resize(1);
            result[0] = static_cast<char>(cp);
        } else if (cp <= 0x7FF) {
            result.resize(2);
            result[1] = static_cast<char>(0x80 | (0x3f & cp));
            result[0] = static_cast<char>(0xC0 | (0x1f & (cp >> 6)));
        } else if (cp <= 0xFFFF) {
            result.resize(3);
            result[2] = static_cast<char>(0x80 | (0x3f & cp));
            result[1] = static_cast<char>(0x80 | (0x3f & (cp >> 6)));
            result[0] = static_cast<char>(0xE0 | (0xf & (cp >> 12)));
        } else if (cp <= 0x10FFFF) {
            result.resize(4);
            result[3] = static_cast<char>(0x80 | (0x3f & cp));
            result[2] = static_cast<char>(0x80 | (0x3f & (cp >> 6)));
            result[1] = static_cast<char>(0x80 | (0x3f & (cp >> 12)));
            result[0] = static_cast<char>(0xF0 | (0x7 & (cp >> 18)));
        }

        return result;
    }

    enum {
        /// Constant that specify the size of the buffer that must be passed to
        /// uintToString.
        uintToStringBufferSize = 3 * sizeof(LargestUInt) + 1
    };

    // Defines a char buffer for use with uintToString().
    using UIntToStringBuffer = char[uintToStringBufferSize];

    /** Converts an unsigned integer to string.
     * @param value Unsigned integer to convert to string
     * @param current Input/Output string buffer.
     *        Must have at least uintToStringBufferSize chars free.
     */
    static inline void uintToString(LargestUInt value, char*& current) {
        *--current = 0;
        do {
            *--current = static_cast<char>(value % 10U + static_cast<unsigned>('0'));
            value /= 10;
        } while (value != 0);
    }

    /** Change ',' to '.' everywhere in buffer.
     *
     * We had a sophisticated way, but it did not work in WinCE.
     * @see https://github.com/open-source-parsers/jsoncpp/pull/9
     */
    template <typename Iter> Iter fixNumericLocale(Iter begin, Iter end) {
        for (; begin != end; ++begin) {
            if (*begin == ',') {
                *begin = '.';
            }
        }
        return begin;
    }

    template <typename Iter> void fixNumericLocaleInput(Iter begin, Iter end) {
        char decimalPoint = getDecimalPoint();
        if (decimalPoint == '\0' || decimalPoint == '.') {
            return;
        }
        for (; begin != end; ++begin) {
            if (*begin == '.') {
                *begin = decimalPoint;
            }
        }
    }

    /**
     * Return iterator that would be the new end of the range [begin,end), if we
     * were to delete zeros in the end of string, but not the last zero before '.'.
     */
    template <typename Iter> Iter fixZerosInTheEnd(Iter begin, Iter end) {
        for (; begin != end; --end) {
            if (*(end - 1) != '0') {
                return end;
            }
            // Don't delete the last zero before the decimal point.
            if (begin != (end - 1) && *(end - 2) == '.') {
                return end;
            }
        }
        return end;
    }

} // namespace Json

#endif // LIB_JSONCPP_JSON_TOOL_H_INCLUDED

// //////////////////////////////////////////////////////////////////////
// End of content of file: src/lib_json/json_tool.h
// //////////////////////////////////////////////////////////////////////






// //////////////////////////////////////////////////////////////////////
// Beginning of content of file: src/lib_json/json_reader.cpp
// //////////////////////////////////////////////////////////////////////

// Copyright 2007-2011 Baptiste Lepilleur and The JsonCpp Authors
// Copyright (C) 2016 InfoTeCS JSC. All rights reserved.
// Distributed under MIT license, or public domain if desired and
// recognized in your jurisdiction.
// See file LICENSE for detail or copy at http://jsoncpp.sourceforge.net/LICENSE

#if !defined(JSON_IS_AMALGAMATION)
#include "json_tool.h"
#include <json/assertions.h>
#include <json/reader.h>
#include <json/value.h>
#endif // if !defined(JSON_IS_AMALGAMATION)
#include <cassert>
#include <cstring>
#include <iostream>
#include <istream>
#include <limits>
#include <memory>
#include <set>
#include <sstream>
#include <utility>

#include <cstdio>
#if __cplusplus >= 201103L

#if !defined(sscanf)
#define sscanf std::sscanf
#endif

#endif //__cplusplus

#if defined(_MSC_VER)
#if !defined(_CRT_SECURE_CPP_OVERLOAD_STANDARD_NAMES)
#define _CRT_SECURE_CPP_OVERLOAD_STANDARD_NAMES 1
#endif //_CRT_SECURE_CPP_OVERLOAD_STANDARD_NAMES
#endif //_MSC_VER

#if defined(_MSC_VER)
// Disable warning about strdup being deprecated.
#pragma warning(disable : 4996)
#endif

// Define JSONCPP_DEPRECATED_STACK_LIMIT as an appropriate integer at compile
// time to change the stack limit
#if !defined(JSONCPP_DEPRECATED_STACK_LIMIT)
#define JSONCPP_DEPRECATED_STACK_LIMIT 1000
#endif

static size_t const stackLimit_g =
JSONCPP_DEPRECATED_STACK_LIMIT; // see readValue()

namespace Json {

#if __cplusplus >= 201103L || (defined(_CPPLIB_VER) && _CPPLIB_VER >= 520)
    using CharReaderPtr = std::unique_ptr<CharReader>;
#else
    using CharReaderPtr = std::auto_ptr<CharReader>;
#endif

    // Implementation of class Features
    // ////////////////////////////////

    Features::Features() = default;

    Features Features::all() { return {}; }

    Features Features::strictMode() {
        Features features;
        features.allowComments_ = false;
        features.allowTrailingCommas_ = false;
        features.strictRoot_ = true;
        features.allowDroppedNullPlaceholders_ = false;
        features.allowNumericKeys_ = false;
        return features;
    }

    // Implementation of class Reader
    // ////////////////////////////////

    bool Reader::containsNewLine(Reader::Location begin, Reader::Location end) {
        for (; begin < end; ++begin)
            if (*begin == '\n' || *begin == '\r')
                return true;
        return false;
    }

    // Class Reader
    // //////////////////////////////////////////////////////////////////

    Reader::Reader() : features_(Features::all()) {}

    Reader::Reader(const Features& features) : features_(features) {}

    bool Reader::parse(const std::string& document, Value& root,
        bool collectComments) {
        document_.assign(document.begin(), document.end());
        const char* begin = document_.c_str();
        const char* end = begin + document_.length();
        return parse(begin, end, root, collectComments);
    }

    bool Reader::parse(std::istream& is, Value& root, bool collectComments) {
        // std::istream_iterator<char> begin(is);
        // std::istream_iterator<char> end;
        // Those would allow streamed input from a file, if parse() were a
        // template function.

        // Since String is reference-counted, this at least does not
        // create an extra copy.
        String doc;
        std::getline(is, doc, static_cast<char> EOF);
        return parse(doc.data(), doc.data() + doc.size(), root, collectComments);
    }

    bool Reader::parse(const char* beginDoc, const char* endDoc, Value& root,
        bool collectComments) {
        if (!features_.allowComments_) {
            collectComments = false;
        }

        begin_ = beginDoc;
        end_ = endDoc;
        collectComments_ = collectComments;
        current_ = begin_;
        lastValueEnd_ = nullptr;
        lastValue_ = nullptr;
        commentsBefore_.clear();
        errors_.clear();
        while (!nodes_.empty())
            nodes_.pop();
        nodes_.push(&root);

        bool successful = readValue();
        Token token;
        skipCommentTokens(token);
        if (collectComments_ && !commentsBefore_.empty())
            root.setComment(commentsBefore_, commentAfter);
        if (features_.strictRoot_) {
            if (!root.isArray() && !root.isObject()) {
                // Set error location to start of doc, ideally should be first token found
                // in doc
                token.type_ = tokenError;
                token.start_ = beginDoc;
                token.end_ = endDoc;
                addError(
                    "A valid JSON document must be either an array or an object value.",
                    token);
                return false;
            }
        }
        return successful;
    }

    bool Reader::readValue() {
        // readValue() may call itself only if it calls readObject() or ReadArray().
        // These methods execute nodes_.push() just before and nodes_.pop)() just
        // after calling readValue(). parse() executes one nodes_.push(), so > instead
        // of >=.
        if (nodes_.size() > stackLimit_g)
            throwRuntimeError("Exceeded stackLimit in readValue().");

        Token token;
        skipCommentTokens(token);
        bool successful = true;

        if (collectComments_ && !commentsBefore_.empty()) {
            currentValue().setComment(commentsBefore_, commentBefore);
            commentsBefore_.clear();
        }

        switch (token.type_) {
        case tokenObjectBegin:
            successful = readObject(token);
            currentValue().setOffsetLimit(current_ - begin_);
            break;
        case tokenArrayBegin:
            successful = readArray(token);
            currentValue().setOffsetLimit(current_ - begin_);
            break;
        case tokenNumber:
            successful = decodeNumber(token);
            break;
        case tokenString:
            successful = decodeString(token);
            break;
        case tokenTrue: {
            Value v(true);
            currentValue().swapPayload(v);
            currentValue().setOffsetStart(token.start_ - begin_);
            currentValue().setOffsetLimit(token.end_ - begin_);
        } break;
        case tokenFalse: {
            Value v(false);
            currentValue().swapPayload(v);
            currentValue().setOffsetStart(token.start_ - begin_);
            currentValue().setOffsetLimit(token.end_ - begin_);
        } break;
        case tokenNull: {
            Value v;
            currentValue().swapPayload(v);
            currentValue().setOffsetStart(token.start_ - begin_);
            currentValue().setOffsetLimit(token.end_ - begin_);
        } break;
        case tokenArraySeparator:
        case tokenObjectEnd:
        case tokenArrayEnd:
            if (features_.allowDroppedNullPlaceholders_) {
                // "Un-read" the current token and mark the current value as a null
                // token.
                current_--;
                Value v;
                currentValue().swapPayload(v);
                currentValue().setOffsetStart(current_ - begin_ - 1);
                currentValue().setOffsetLimit(current_ - begin_);
                break;
            } // Else, fall through...
        default:
            currentValue().setOffsetStart(token.start_ - begin_);
            currentValue().setOffsetLimit(token.end_ - begin_);
            return addError("Syntax error: value, object or array expected.", token);
        }

        if (collectComments_) {
            lastValueEnd_ = current_;
            lastValue_ = &currentValue();
        }

        return successful;
    }

    void Reader::skipCommentTokens(Token& token) {
        if (features_.allowComments_) {
            do {
                readToken(token);
            } while (token.type_ == tokenComment);
        } else {
            readToken(token);
        }
    }

    bool Reader::readToken(Token& token) {
        skipSpaces();
        token.start_ = current_;
        Char c = getNextChar();
        bool ok = true;
        switch (c) {
        case '{':
            token.type_ = tokenObjectBegin;
            break;
        case '}':
            token.type_ = tokenObjectEnd;
            break;
        case '[':
            token.type_ = tokenArrayBegin;
            break;
        case ']':
            token.type_ = tokenArrayEnd;
            break;
        case '"':
            token.type_ = tokenString;
            ok = readString();
            break;
        case '/':
            token.type_ = tokenComment;
            ok = readComment();
            break;
        case '0':
        case '1':
        case '2':
        case '3':
        case '4':
        case '5':
        case '6':
        case '7':
        case '8':
        case '9':
        case '-':
            token.type_ = tokenNumber;
            readNumber();
            break;
        case 't':
            token.type_ = tokenTrue;
            ok = match("rue", 3);
            break;
        case 'f':
            token.type_ = tokenFalse;
            ok = match("alse", 4);
            break;
        case 'n':
            token.type_ = tokenNull;
            ok = match("ull", 3);
            break;
        case ',':
            token.type_ = tokenArraySeparator;
            break;
        case ':':
            token.type_ = tokenMemberSeparator;
            break;
        case 0:
            token.type_ = tokenEndOfStream;
            break;
        default:
            ok = false;
            break;
        }
        if (!ok)
            token.type_ = tokenError;
        token.end_ = current_;
        return ok;
    }

    void Reader::skipSpaces() {
        while (current_ != end_) {
            Char c = *current_;
            if (c == ' ' || c == '\t' || c == '\r' || c == '\n')
                ++current_;
            else
                break;
        }
    }

    bool Reader::match(const Char* pattern, int patternLength) {
        if (end_ - current_ < patternLength)
            return false;
        int index = patternLength;
        while (index--)
            if (current_[index] != pattern[index])
                return false;
        current_ += patternLength;
        return true;
    }

    bool Reader::readComment() {
        Location commentBegin = current_ - 1;
        Char c = getNextChar();
        bool successful = false;
        if (c == '*')
            successful = readCStyleComment();
        else if (c == '/')
            successful = readCppStyleComment();
        if (!successful)
            return false;

        if (collectComments_) {
            CommentPlacement placement = commentBefore;
            if (lastValueEnd_ && !containsNewLine(lastValueEnd_, commentBegin)) {
                if (c != '*' || !containsNewLine(commentBegin, current_))
                    placement = commentAfterOnSameLine;
            }

            addComment(commentBegin, current_, placement);
        }
        return true;
    }

    String Reader::normalizeEOL(Reader::Location begin, Reader::Location end) {
        String normalized;
        normalized.reserve(static_cast<size_t>(end - begin));
        Reader::Location current = begin;
        while (current != end) {
            char c = *current++;
            if (c == '\r') {
                if (current != end && *current == '\n')
                    // convert dos EOL
                    ++current;
                // convert Mac EOL
                normalized += '\n';
            } else {
                normalized += c;
            }
        }
        return normalized;
    }

    void Reader::addComment(Location begin, Location end,
        CommentPlacement placement) {
        assert(collectComments_);
        const String& normalized = normalizeEOL(begin, end);
        if (placement == commentAfterOnSameLine) {
            assert(lastValue_ != nullptr);
            lastValue_->setComment(normalized, placement);
        } else {
            commentsBefore_ += normalized;
        }
    }

    bool Reader::readCStyleComment() {
        while ((current_ + 1) < end_) {
            Char c = getNextChar();
            if (c == '*' && *current_ == '/')
                break;
        }
        return getNextChar() == '/';
    }

    bool Reader::readCppStyleComment() {
        while (current_ != end_) {
            Char c = getNextChar();
            if (c == '\n')
                break;
            if (c == '\r') {
                // Consume DOS EOL. It will be normalized in addComment.
                if (current_ != end_ && *current_ == '\n')
                    getNextChar();
                // Break on Moc OS 9 EOL.
                break;
            }
        }
        return true;
    }

    void Reader::readNumber() {
        Location p = current_;
        char c = '0'; // stopgap for already consumed character
        // integral part
        while (c >= '0' && c <= '9')
            c = (current_ = p) < end_ ? *p++ : '\0';
        // fractional part
        if (c == '.') {
            c = (current_ = p) < end_ ? *p++ : '\0';
            while (c >= '0' && c <= '9')
                c = (current_ = p) < end_ ? *p++ : '\0';
        }
        // exponential part
        if (c == 'e' || c == 'E') {
            c = (current_ = p) < end_ ? *p++ : '\0';
            if (c == '+' || c == '-')
                c = (current_ = p) < end_ ? *p++ : '\0';
            while (c >= '0' && c <= '9')
                c = (current_ = p) < end_ ? *p++ : '\0';
        }
    }

    bool Reader::readString() {
        Char c = '\0';
        while (current_ != end_) {
            c = getNextChar();
            if (c == '\\')
                getNextChar();
            else if (c == '"')
                break;
        }
        return c == '"';
    }

    bool Reader::readObject(Token& token) {
        Token tokenName;
        String name;
        Value init(objectValue);
        currentValue().swapPayload(init);
        currentValue().setOffsetStart(token.start_ - begin_);
        while (readToken(tokenName)) {
            bool initialTokenOk = true;
            while (tokenName.type_ == tokenComment && initialTokenOk)
                initialTokenOk = readToken(tokenName);
            if (!initialTokenOk)
                break;
            if (tokenName.type_ == tokenObjectEnd &&
                (name.empty() ||
                    features_.allowTrailingCommas_)) // empty object or trailing comma
                return true;
            name.clear();
            if (tokenName.type_ == tokenString) {
                if (!decodeString(tokenName, name))
                    return recoverFromError(tokenObjectEnd);
            } else if (tokenName.type_ == tokenNumber && features_.allowNumericKeys_) {
                Value numberName;
                if (!decodeNumber(tokenName, numberName))
                    return recoverFromError(tokenObjectEnd);
                name = numberName.asString();
            } else {
                break;
            }

            Token colon;
            if (!readToken(colon) || colon.type_ != tokenMemberSeparator) {
                return addErrorAndRecover("Missing ':' after object member name", colon,
                    tokenObjectEnd);
            }
            Value& value = currentValue()[name];
            nodes_.push(&value);
            bool ok = readValue();
            nodes_.pop();
            if (!ok) // error already set
                return recoverFromError(tokenObjectEnd);

            Token comma;
            if (!readToken(comma) ||
                (comma.type_ != tokenObjectEnd && comma.type_ != tokenArraySeparator &&
                    comma.type_ != tokenComment)) {
                return addErrorAndRecover("Missing ',' or '}' in object declaration",
                    comma, tokenObjectEnd);
            }
            bool finalizeTokenOk = true;
            while (comma.type_ == tokenComment && finalizeTokenOk)
                finalizeTokenOk = readToken(comma);
            if (comma.type_ == tokenObjectEnd)
                return true;
        }
        return addErrorAndRecover("Missing '}' or object member name", tokenName,
            tokenObjectEnd);
    }

    bool Reader::readArray(Token& token) {
        Value init(arrayValue);
        currentValue().swapPayload(init);
        currentValue().setOffsetStart(token.start_ - begin_);
        int index = 0;
        for (;;) {
            skipSpaces();
            if (current_ != end_ && *current_ == ']' &&
                (index == 0 ||
                (features_.allowTrailingCommas_ &&
                    !features_.allowDroppedNullPlaceholders_))) // empty array or trailing
                                                                // comma
            {
                Token endArray;
                readToken(endArray);
                return true;
            }

            Value& value = currentValue()[index++];
            nodes_.push(&value);
            bool ok = readValue();
            nodes_.pop();
            if (!ok) // error already set
                return recoverFromError(tokenArrayEnd);

            Token currentToken;
            // Accept Comment after last item in the array.
            ok = readToken(currentToken);
            while (currentToken.type_ == tokenComment && ok) {
                ok = readToken(currentToken);
            }
            bool badTokenType = (currentToken.type_ != tokenArraySeparator &&
                currentToken.type_ != tokenArrayEnd);
            if (!ok || badTokenType) {
                return addErrorAndRecover("Missing ',' or ']' in array declaration",
                    currentToken, tokenArrayEnd);
            }
            if (currentToken.type_ == tokenArrayEnd)
                break;
        }
        return true;
    }

    bool Reader::decodeNumber(Token& token) {
        Value decoded;
        if (!decodeNumber(token, decoded))
            return false;
        currentValue().swapPayload(decoded);
        currentValue().setOffsetStart(token.start_ - begin_);
        currentValue().setOffsetLimit(token.end_ - begin_);
        return true;
    }

    bool Reader::decodeNumber(Token& token, Value& decoded) {
        // Attempts to parse the number as an integer. If the number is
        // larger than the maximum supported value of an integer then
        // we decode the number as a double.
        Location current = token.start_;
        bool isNegative = *current == '-';
        if (isNegative)
            ++current;
        // TODO: Help the compiler do the div and mod at compile time or get rid of
        // them.
        Value::LargestUInt maxIntegerValue =
            isNegative ? Value::LargestUInt(Value::maxLargestInt) + 1
            : Value::maxLargestUInt;
        Value::LargestUInt threshold = maxIntegerValue / 10;
        Value::LargestUInt value = 0;
        while (current < token.end_) {
            Char c = *current++;
            if (c < '0' || c > '9')
                return decodeDouble(token, decoded);
            auto digit(static_cast<Value::UInt>(c - '0'));
            if (value >= threshold) {
                // We've hit or exceeded the max value divided by 10 (rounded down). If
                // a) we've only just touched the limit, b) this is the last digit, and
                // c) it's small enough to fit in that rounding delta, we're okay.
                // Otherwise treat this number as a double to avoid overflow.
                if (value > threshold || current != token.end_ ||
                    digit > maxIntegerValue % 10) {
                    return decodeDouble(token, decoded);
                }
            }
            value = value * 10 + digit;
        }
        if (isNegative && value == maxIntegerValue)
            decoded = Value::minLargestInt;
        else if (isNegative)
            decoded = -Value::LargestInt(value);
        else if (value <= Value::LargestUInt(Value::maxInt))
            decoded = Value::LargestInt(value);
        else
            decoded = value;
        return true;
    }

    bool Reader::decodeDouble(Token& token) {
        Value decoded;
        if (!decodeDouble(token, decoded))
            return false;
        currentValue().swapPayload(decoded);
        currentValue().setOffsetStart(token.start_ - begin_);
        currentValue().setOffsetLimit(token.end_ - begin_);
        return true;
    }

    bool Reader::decodeDouble(Token& token, Value& decoded) {
        double value = 0;
        String buffer(token.start_, token.end_);
        IStringStream is(buffer);
        if (!(is >> value))
            return addError(
                "'" + String(token.start_, token.end_) + "' is not a number.", token);
        decoded = value;
        return true;
    }

    bool Reader::decodeString(Token& token) {
        String decoded_string;
        if (!decodeString(token, decoded_string))
            return false;
        Value decoded(decoded_string);
        currentValue().swapPayload(decoded);
        currentValue().setOffsetStart(token.start_ - begin_);
        currentValue().setOffsetLimit(token.end_ - begin_);
        return true;
    }

    bool Reader::decodeString(Token& token, String& decoded) {
        decoded.reserve(static_cast<size_t>(token.end_ - token.start_ - 2));
        Location current = token.start_ + 1; // skip '"'
        Location end = token.end_ - 1;       // do not include '"'
        while (current != end) {
            Char c = *current++;
            if (c == '"')
                break;
            if (c == '\\') {
                if (current == end)
                    return addError("Empty escape sequence in string", token, current);
                Char escape = *current++;
                switch (escape) {
                case '"':
                    decoded += '"';
                    break;
                case '/':
                    decoded += '/';
                    break;
                case '\\':
                    decoded += '\\';
                    break;
                case 'b':
                    decoded += '\b';
                    break;
                case 'f':
                    decoded += '\f';
                    break;
                case 'n':
                    decoded += '\n';
                    break;
                case 'r':
                    decoded += '\r';
                    break;
                case 't':
                    decoded += '\t';
                    break;
                case 'u': {
                    unsigned int unicode;
                    if (!decodeUnicodeCodePoint(token, current, end, unicode))
                        return false;
                    decoded += codePointToUTF8(unicode);
                } break;
                default:
                    return addError("Bad escape sequence in string", token, current);
                }
            } else {
                decoded += c;
            }
        }
        return true;
    }

    bool Reader::decodeUnicodeCodePoint(Token& token, Location& current,
        Location end, unsigned int& unicode) {

        if (!decodeUnicodeEscapeSequence(token, current, end, unicode))
            return false;
        if (unicode >= 0xD800 && unicode <= 0xDBFF) {
            // surrogate pairs
            if (end - current < 6)
                return addError(
                    "additional six characters expected to parse unicode surrogate pair.",
                    token, current);
            if (*(current++) == '\\' && *(current++) == 'u') {
                unsigned int surrogatePair;
                if (decodeUnicodeEscapeSequence(token, current, end, surrogatePair)) {
                    unicode = 0x10000 + ((unicode & 0x3FF) << 10) + (surrogatePair & 0x3FF);
                } else
                    return false;
            } else
                return addError("expecting another \\u token to begin the second half of "
                    "a unicode surrogate pair",
                    token, current);
        }
        return true;
    }

    bool Reader::decodeUnicodeEscapeSequence(Token& token, Location& current,
        Location end,
        unsigned int& ret_unicode) {
        if (end - current < 4)
            return addError(
                "Bad unicode escape sequence in string: four digits expected.", token,
                current);
        int unicode = 0;
        for (int index = 0; index < 4; ++index) {
            Char c = *current++;
            unicode *= 16;
            if (c >= '0' && c <= '9')
                unicode += c - '0';
            else if (c >= 'a' && c <= 'f')
                unicode += c - 'a' + 10;
            else if (c >= 'A' && c <= 'F')
                unicode += c - 'A' + 10;
            else
                return addError(
                    "Bad unicode escape sequence in string: hexadecimal digit expected.",
                    token, current);
        }
        ret_unicode = static_cast<unsigned int>(unicode);
        return true;
    }

    bool Reader::addError(const String& message, Token& token, Location extra) {
        ErrorInfo info;
        info.token_ = token;
        info.message_ = message;
        info.extra_ = extra;
        errors_.push_back(info);
        return false;
    }

    bool Reader::recoverFromError(TokenType skipUntilToken) {
        size_t const errorCount = errors_.size();
        Token skip;
        for (;;) {
            if (!readToken(skip))
                errors_.resize(errorCount); // discard errors caused by recovery
            if (skip.type_ == skipUntilToken || skip.type_ == tokenEndOfStream)
                break;
        }
        errors_.resize(errorCount);
        return false;
    }

    bool Reader::addErrorAndRecover(const String& message, Token& token,
        TokenType skipUntilToken) {
        addError(message, token);
        return recoverFromError(skipUntilToken);
    }

    Value& Reader::currentValue() { return *(nodes_.top()); }

    Reader::Char Reader::getNextChar() {
        if (current_ == end_)
            return 0;
        return *current_++;
    }

    void Reader::getLocationLineAndColumn(Location location, int& line,
        int& column) const {
        Location current = begin_;
        Location lastLineStart = current;
        line = 0;
        while (current < location && current != end_) {
            Char c = *current++;
            if (c == '\r') {
                if (*current == '\n')
                    ++current;
                lastLineStart = current;
                ++line;
            } else if (c == '\n') {
                lastLineStart = current;
                ++line;
            }
        }
        // column & line start at 1
        column = int(location - lastLineStart) + 1;
        ++line;
    }

    String Reader::getLocationLineAndColumn(Location location) const {
        int line, column;
        getLocationLineAndColumn(location, line, column);
        char buffer[18 + 16 + 16 + 1];
        jsoncpp_snprintf(buffer, sizeof(buffer), "Line %d, Column %d", line, column);
        return buffer;
    }

    // Deprecated. Preserved for backward compatibility
    String Reader::getFormatedErrorMessages() const {
        return getFormattedErrorMessages();
    }

    String Reader::getFormattedErrorMessages() const {
        String formattedMessage;
        for (const auto& error : errors_) {
            formattedMessage +=
                "* " + getLocationLineAndColumn(error.token_.start_) + "\n";
            formattedMessage += "  " + error.message_ + "\n";
            if (error.extra_)
                formattedMessage +=
                "See " + getLocationLineAndColumn(error.extra_) + " for detail.\n";
        }
        return formattedMessage;
    }

    std::vector<Reader::StructuredError> Reader::getStructuredErrors() const {
        std::vector<Reader::StructuredError> allErrors;
        for (const auto& error : errors_) {
            Reader::StructuredError structured;
            structured.offset_start = error.token_.start_ - begin_;
            structured.offset_limit = error.token_.end_ - begin_;
            structured.message = error.message_;
            allErrors.push_back(structured);
        }
        return allErrors;
    }

    bool Reader::pushError(const Value& value, const String& message) {
        ptrdiff_t const length = end_ - begin_;
        if (value.getOffsetStart() > length || value.getOffsetLimit() > length)
            return false;
        Token token;
        token.type_ = tokenError;
        token.start_ = begin_ + value.getOffsetStart();
        token.end_ = begin_ + value.getOffsetLimit();
        ErrorInfo info;
        info.token_ = token;
        info.message_ = message;
        info.extra_ = nullptr;
        errors_.push_back(info);
        return true;
    }

    bool Reader::pushError(const Value& value, const String& message,
        const Value& extra) {
        ptrdiff_t const length = end_ - begin_;
        if (value.getOffsetStart() > length || value.getOffsetLimit() > length ||
            extra.getOffsetLimit() > length)
            return false;
        Token token;
        token.type_ = tokenError;
        token.start_ = begin_ + value.getOffsetStart();
        token.end_ = begin_ + value.getOffsetLimit();
        ErrorInfo info;
        info.token_ = token;
        info.message_ = message;
        info.extra_ = begin_ + extra.getOffsetStart();
        errors_.push_back(info);
        return true;
    }

    bool Reader::good() const { return errors_.empty(); }

    // Originally copied from the Features class (now deprecated), used internally
    // for features implementation.
    class OurFeatures {
    public:
        static OurFeatures all();
        bool allowComments_;
        bool allowTrailingCommas_;
        bool strictRoot_;
        bool allowDroppedNullPlaceholders_;
        bool allowNumericKeys_;
        bool allowSingleQuotes_;
        bool failIfExtra_;
        bool rejectDupKeys_;
        bool allowSpecialFloats_;
        size_t stackLimit_;
    }; // OurFeatures

    OurFeatures OurFeatures::all() { return {}; }

    // Implementation of class Reader
    // ////////////////////////////////

    // Originally copied from the Reader class (now deprecated), used internally
    // for implementing JSON reading.
    class OurReader {
    public:
        using Char = char;
        using Location = const Char*;
        struct StructuredError {
            ptrdiff_t offset_start;
            ptrdiff_t offset_limit;
            String message;
        };

        explicit OurReader(OurFeatures const& features);
        bool parse(const char* beginDoc, const char* endDoc, Value& root,
            bool collectComments = true);
        String getFormattedErrorMessages() const;
        std::vector<StructuredError> getStructuredErrors() const;

    private:
        OurReader(OurReader const&);      // no impl
        void operator=(OurReader const&); // no impl

        enum TokenType {
            tokenEndOfStream = 0,
            tokenObjectBegin,
            tokenObjectEnd,
            tokenArrayBegin,
            tokenArrayEnd,
            tokenString,
            tokenNumber,
            tokenTrue,
            tokenFalse,
            tokenNull,
            tokenNaN,
            tokenPosInf,
            tokenNegInf,
            tokenArraySeparator,
            tokenMemberSeparator,
            tokenComment,
            tokenError
        };

        class Token {
        public:
            TokenType type_;
            Location start_;
            Location end_;
        };

        class ErrorInfo {
        public:
            Token token_;
            String message_;
            Location extra_;
        };

        using Errors = std::deque<ErrorInfo>;

        bool readToken(Token& token);
        void skipSpaces();
        bool match(const Char* pattern, int patternLength);
        bool readComment();
        bool readCStyleComment(bool* containsNewLineResult);
        bool readCppStyleComment();
        bool readString();
        bool readStringSingleQuote();
        bool readNumber(bool checkInf);
        bool readValue();
        bool readObject(Token& token);
        bool readArray(Token& token);
        bool decodeNumber(Token& token);
        bool decodeNumber(Token& token, Value& decoded);
        bool decodeString(Token& token);
        bool decodeString(Token& token, String& decoded);
        bool decodeDouble(Token& token);
        bool decodeDouble(Token& token, Value& decoded);
        bool decodeUnicodeCodePoint(Token& token, Location& current, Location end,
            unsigned int& unicode);
        bool decodeUnicodeEscapeSequence(Token& token, Location& current,
            Location end, unsigned int& unicode);
        bool addError(const String& message, Token& token, Location extra = nullptr);
        bool recoverFromError(TokenType skipUntilToken);
        bool addErrorAndRecover(const String& message, Token& token,
            TokenType skipUntilToken);
        void skipUntilSpace();
        Value& currentValue();
        Char getNextChar();
        void getLocationLineAndColumn(Location location, int& line,
            int& column) const;
        String getLocationLineAndColumn(Location location) const;
        void addComment(Location begin, Location end, CommentPlacement placement);
        void skipCommentTokens(Token& token);

        static String normalizeEOL(Location begin, Location end);
        static bool containsNewLine(Location begin, Location end);

        using Nodes = std::stack<Value*>;

        Nodes nodes_{};
        Errors errors_{};
        String document_{};
        Location begin_ = nullptr;
        Location end_ = nullptr;
        Location current_ = nullptr;
        Location lastValueEnd_ = nullptr;
        Value* lastValue_ = nullptr;
        bool lastValueHasAComment_ = false;
        String commentsBefore_{};

        OurFeatures const features_;
        bool collectComments_ = false;
    }; // OurReader

    // complete copy of Read impl, for OurReader

    bool OurReader::containsNewLine(OurReader::Location begin,
        OurReader::Location end) {
        for (; begin < end; ++begin)
            if (*begin == '\n' || *begin == '\r')
                return true;
        return false;
    }

    OurReader::OurReader(OurFeatures const& features) : features_(features) {}

    bool OurReader::parse(const char* beginDoc, const char* endDoc, Value& root,
        bool collectComments) {
        if (!features_.allowComments_) {
            collectComments = false;
        }

        begin_ = beginDoc;
        end_ = endDoc;
        collectComments_ = collectComments;
        current_ = begin_;
        lastValueEnd_ = nullptr;
        lastValue_ = nullptr;
        commentsBefore_.clear();
        errors_.clear();
        while (!nodes_.empty())
            nodes_.pop();
        nodes_.push(&root);

        bool successful = readValue();
        nodes_.pop();
        Token token;
        skipCommentTokens(token);
        if (features_.failIfExtra_ && (token.type_ != tokenEndOfStream)) {
            addError("Extra non-whitespace after JSON value.", token);
            return false;
        }
        if (collectComments_ && !commentsBefore_.empty())
            root.setComment(commentsBefore_, commentAfter);
        if (features_.strictRoot_) {
            if (!root.isArray() && !root.isObject()) {
                // Set error location to start of doc, ideally should be first token found
                // in doc
                token.type_ = tokenError;
                token.start_ = beginDoc;
                token.end_ = endDoc;
                addError(
                    "A valid JSON document must be either an array or an object value.",
                    token);
                return false;
            }
        }
        return successful;
    }

    bool OurReader::readValue() {
        //  To preserve the old behaviour we cast size_t to int.
        if (nodes_.size() > features_.stackLimit_)
            throwRuntimeError("Exceeded stackLimit in readValue().");
        Token token;
        skipCommentTokens(token);
        bool successful = true;

        if (collectComments_ && !commentsBefore_.empty()) {
            currentValue().setComment(commentsBefore_, commentBefore);
            commentsBefore_.clear();
        }

        switch (token.type_) {
        case tokenObjectBegin:
            successful = readObject(token);
            currentValue().setOffsetLimit(current_ - begin_);
            break;
        case tokenArrayBegin:
            successful = readArray(token);
            currentValue().setOffsetLimit(current_ - begin_);
            break;
        case tokenNumber:
            successful = decodeNumber(token);
            break;
        case tokenString:
            successful = decodeString(token);
            break;
        case tokenTrue: {
            Value v(true);
            currentValue().swapPayload(v);
            currentValue().setOffsetStart(token.start_ - begin_);
            currentValue().setOffsetLimit(token.end_ - begin_);
        } break;
        case tokenFalse: {
            Value v(false);
            currentValue().swapPayload(v);
            currentValue().setOffsetStart(token.start_ - begin_);
            currentValue().setOffsetLimit(token.end_ - begin_);
        } break;
        case tokenNull: {
            Value v;
            currentValue().swapPayload(v);
            currentValue().setOffsetStart(token.start_ - begin_);
            currentValue().setOffsetLimit(token.end_ - begin_);
        } break;
        case tokenNaN: {
            Value v(std::numeric_limits<double>::quiet_NaN());
            currentValue().swapPayload(v);
            currentValue().setOffsetStart(token.start_ - begin_);
            currentValue().setOffsetLimit(token.end_ - begin_);
        } break;
        case tokenPosInf: {
            Value v(std::numeric_limits<double>::infinity());
            currentValue().swapPayload(v);
            currentValue().setOffsetStart(token.start_ - begin_);
            currentValue().setOffsetLimit(token.end_ - begin_);
        } break;
        case tokenNegInf: {
            Value v(-std::numeric_limits<double>::infinity());
            currentValue().swapPayload(v);
            currentValue().setOffsetStart(token.start_ - begin_);
            currentValue().setOffsetLimit(token.end_ - begin_);
        } break;
        case tokenArraySeparator:
        case tokenObjectEnd:
        case tokenArrayEnd:
            if (features_.allowDroppedNullPlaceholders_) {
                // "Un-read" the current token and mark the current value as a null
                // token.
                current_--;
                Value v;
                currentValue().swapPayload(v);
                currentValue().setOffsetStart(current_ - begin_ - 1);
                currentValue().setOffsetLimit(current_ - begin_);
                break;
            } // else, fall through ...
        default:
            currentValue().setOffsetStart(token.start_ - begin_);
            currentValue().setOffsetLimit(token.end_ - begin_);
            return addError("Syntax error: value, object or array expected.", token);
        }

        if (collectComments_) {
            lastValueEnd_ = current_;
            lastValueHasAComment_ = false;
            lastValue_ = &currentValue();
        }

        return successful;
    }

    void OurReader::skipCommentTokens(Token& token) {
        if (features_.allowComments_) {
            do {
                readToken(token);
            } while (token.type_ == tokenComment);
        } else {
            readToken(token);
        }
    }

    bool OurReader::readToken(Token& token) {
        skipSpaces();
        token.start_ = current_;
        Char c = getNextChar();
        bool ok = true;
        switch (c) {
        case '{':
            token.type_ = tokenObjectBegin;
            break;
        case '}':
            token.type_ = tokenObjectEnd;
            break;
        case '[':
            token.type_ = tokenArrayBegin;
            break;
        case ']':
            token.type_ = tokenArrayEnd;
            break;
        case '"':
            token.type_ = tokenString;
            ok = readString();
            break;
        case '\'':
            if (features_.allowSingleQuotes_) {
                token.type_ = tokenString;
                ok = readStringSingleQuote();
                break;
            } // else fall through
        case '/':
            token.type_ = tokenComment;
            ok = readComment();
            break;
        case '0':
        case '1':
        case '2':
        case '3':
        case '4':
        case '5':
        case '6':
        case '7':
        case '8':
        case '9':
            token.type_ = tokenNumber;
            readNumber(false);
            break;
        case '-':
            if (readNumber(true)) {
                token.type_ = tokenNumber;
            } else {
                token.type_ = tokenNegInf;
                ok = features_.allowSpecialFloats_ && match("nfinity", 7);
            }
            break;
        case '+':
            if (readNumber(true)) {
                token.type_ = tokenNumber;
            } else {
                token.type_ = tokenPosInf;
                ok = features_.allowSpecialFloats_ && match("nfinity", 7);
            }
            break;
        case 't':
            token.type_ = tokenTrue;
            ok = match("rue", 3);
            break;
        case 'f':
            token.type_ = tokenFalse;
            ok = match("alse", 4);
            break;
        case 'n':
            token.type_ = tokenNull;
            ok = match("ull", 3);
            break;
        case 'N':
            if (features_.allowSpecialFloats_) {
                token.type_ = tokenNaN;
                ok = match("aN", 2);
            } else {
                ok = false;
            }
            break;
        case 'I':
            if (features_.allowSpecialFloats_) {
                token.type_ = tokenPosInf;
                ok = match("nfinity", 7);
            } else {
                ok = false;
            }
            break;
        case ',':
            token.type_ = tokenArraySeparator;
            break;
        case ':':
            token.type_ = tokenMemberSeparator;
            break;
        case 0:
            token.type_ = tokenEndOfStream;
            break;
        default:
            ok = false;
            break;
        }
        if (!ok)
            token.type_ = tokenError;
        token.end_ = current_;
        return ok;
    }

    void OurReader::skipSpaces() {
        while (current_ != end_) {
            Char c = *current_;
            if (c == ' ' || c == '\t' || c == '\r' || c == '\n')
                ++current_;
            else
                break;
        }
    }

    bool OurReader::match(const Char* pattern, int patternLength) {
        if (end_ - current_ < patternLength)
            return false;
        int index = patternLength;
        while (index--)
            if (current_[index] != pattern[index])
                return false;
        current_ += patternLength;
        return true;
    }

    bool OurReader::readComment() {
        const Location commentBegin = current_ - 1;
        const Char c = getNextChar();
        bool successful = false;
        bool cStyleWithEmbeddedNewline = false;

        const bool isCStyleComment = (c == '*');
        const bool isCppStyleComment = (c == '/');
        if (isCStyleComment) {
            successful = readCStyleComment(&cStyleWithEmbeddedNewline);
        } else if (isCppStyleComment) {
            successful = readCppStyleComment();
        }

        if (!successful)
            return false;

        if (collectComments_) {
            CommentPlacement placement = commentBefore;

            if (!lastValueHasAComment_) {
                if (lastValueEnd_ && !containsNewLine(lastValueEnd_, commentBegin)) {
                    if (isCppStyleComment || !cStyleWithEmbeddedNewline) {
                        placement = commentAfterOnSameLine;
                        lastValueHasAComment_ = true;
                    }
                }
            }

            addComment(commentBegin, current_, placement);
        }
        return true;
    }

    String OurReader::normalizeEOL(OurReader::Location begin,
        OurReader::Location end) {
        String normalized;
        normalized.reserve(static_cast<size_t>(end - begin));
        OurReader::Location current = begin;
        while (current != end) {
            char c = *current++;
            if (c == '\r') {
                if (current != end && *current == '\n')
                    // convert dos EOL
                    ++current;
                // convert Mac EOL
                normalized += '\n';
            } else {
                normalized += c;
            }
        }
        return normalized;
    }

    void OurReader::addComment(Location begin, Location end,
        CommentPlacement placement) {
        assert(collectComments_);
        const String& normalized = normalizeEOL(begin, end);
        if (placement == commentAfterOnSameLine) {
            assert(lastValue_ != nullptr);
            lastValue_->setComment(normalized, placement);
        } else {
            commentsBefore_ += normalized;
        }
    }

    bool OurReader::readCStyleComment(bool* containsNewLineResult) {
        *containsNewLineResult = false;

        while ((current_ + 1) < end_) {
            Char c = getNextChar();
            if (c == '*' && *current_ == '/')
                break;
            if (c == '\n')
                *containsNewLineResult = true;
        }

        return getNextChar() == '/';
    }

    bool OurReader::readCppStyleComment() {
        while (current_ != end_) {
            Char c = getNextChar();
            if (c == '\n')
                break;
            if (c == '\r') {
                // Consume DOS EOL. It will be normalized in addComment.
                if (current_ != end_ && *current_ == '\n')
                    getNextChar();
                // Break on Moc OS 9 EOL.
                break;
            }
        }
        return true;
    }

    bool OurReader::readNumber(bool checkInf) {
        Location p = current_;
        if (checkInf && p != end_ && *p == 'I') {
            current_ = ++p;
            return false;
        }
        char c = '0'; // stopgap for already consumed character
        // integral part
        while (c >= '0' && c <= '9')
            c = (current_ = p) < end_ ? *p++ : '\0';
        // fractional part
        if (c == '.') {
            c = (current_ = p) < end_ ? *p++ : '\0';
            while (c >= '0' && c <= '9')
                c = (current_ = p) < end_ ? *p++ : '\0';
        }
        // exponential part
        if (c == 'e' || c == 'E') {
            c = (current_ = p) < end_ ? *p++ : '\0';
            if (c == '+' || c == '-')
                c = (current_ = p) < end_ ? *p++ : '\0';
            while (c >= '0' && c <= '9')
                c = (current_ = p) < end_ ? *p++ : '\0';
        }
        return true;
    }
    bool OurReader::readString() {
        Char c = 0;
        while (current_ != end_) {
            c = getNextChar();
            if (c == '\\')
                getNextChar();
            else if (c == '"')
                break;
        }
        return c == '"';
    }

    bool OurReader::readStringSingleQuote() {
        Char c = 0;
        while (current_ != end_) {
            c = getNextChar();
            if (c == '\\')
                getNextChar();
            else if (c == '\'')
                break;
        }
        return c == '\'';
    }

    bool OurReader::readObject(Token& token) {
        Token tokenName;
        String name;
        Value init(objectValue);
        currentValue().swapPayload(init);
        currentValue().setOffsetStart(token.start_ - begin_);
        while (readToken(tokenName)) {
            bool initialTokenOk = true;
            while (tokenName.type_ == tokenComment && initialTokenOk)
                initialTokenOk = readToken(tokenName);
            if (!initialTokenOk)
                break;
            if (tokenName.type_ == tokenObjectEnd &&
                (name.empty() ||
                    features_.allowTrailingCommas_)) // empty object or trailing comma
                return true;
            name.clear();
            if (tokenName.type_ == tokenString) {
                if (!decodeString(tokenName, name))
                    return recoverFromError(tokenObjectEnd);
            } else if (tokenName.type_ == tokenNumber && features_.allowNumericKeys_) {
                Value numberName;
                if (!decodeNumber(tokenName, numberName))
                    return recoverFromError(tokenObjectEnd);
                name = numberName.asString();
            } else {
                break;
            }
            if (name.length() >= (1U << 30))
                throwRuntimeError("keylength >= 2^30");
            if (features_.rejectDupKeys_ && currentValue().isMember(name)) {
                String msg = "Duplicate key: '" + name + "'";
                return addErrorAndRecover(msg, tokenName, tokenObjectEnd);
            }

            Token colon;
            if (!readToken(colon) || colon.type_ != tokenMemberSeparator) {
                return addErrorAndRecover("Missing ':' after object member name", colon,
                    tokenObjectEnd);
            }
            Value& value = currentValue()[name];
            nodes_.push(&value);
            bool ok = readValue();
            nodes_.pop();
            if (!ok) // error already set
                return recoverFromError(tokenObjectEnd);

            Token comma;
            if (!readToken(comma) ||
                (comma.type_ != tokenObjectEnd && comma.type_ != tokenArraySeparator &&
                    comma.type_ != tokenComment)) {
                return addErrorAndRecover("Missing ',' or '}' in object declaration",
                    comma, tokenObjectEnd);
            }
            bool finalizeTokenOk = true;
            while (comma.type_ == tokenComment && finalizeTokenOk)
                finalizeTokenOk = readToken(comma);
            if (comma.type_ == tokenObjectEnd)
                return true;
        }
        return addErrorAndRecover("Missing '}' or object member name", tokenName,
            tokenObjectEnd);
    }

    bool OurReader::readArray(Token& token) {
        Value init(arrayValue);
        currentValue().swapPayload(init);
        currentValue().setOffsetStart(token.start_ - begin_);
        int index = 0;
        for (;;) {
            skipSpaces();
            if (current_ != end_ && *current_ == ']' &&
                (index == 0 ||
                (features_.allowTrailingCommas_ &&
                    !features_.allowDroppedNullPlaceholders_))) // empty array or trailing
                                                                // comma
            {
                Token endArray;
                readToken(endArray);
                return true;
            }
            Value& value = currentValue()[index++];
            nodes_.push(&value);
            bool ok = readValue();
            nodes_.pop();
            if (!ok) // error already set
                return recoverFromError(tokenArrayEnd);

            Token currentToken;
            // Accept Comment after last item in the array.
            ok = readToken(currentToken);
            while (currentToken.type_ == tokenComment && ok) {
                ok = readToken(currentToken);
            }
            bool badTokenType = (currentToken.type_ != tokenArraySeparator &&
                currentToken.type_ != tokenArrayEnd);
            if (!ok || badTokenType) {
                return addErrorAndRecover("Missing ',' or ']' in array declaration",
                    currentToken, tokenArrayEnd);
            }
            if (currentToken.type_ == tokenArrayEnd)
                break;
        }
        return true;
    }

    bool OurReader::decodeNumber(Token& token) {
        Value decoded;
        if (!decodeNumber(token, decoded))
            return false;
        currentValue().swapPayload(decoded);
        currentValue().setOffsetStart(token.start_ - begin_);
        currentValue().setOffsetLimit(token.end_ - begin_);
        return true;
    }

    bool OurReader::decodeNumber(Token& token, Value& decoded) {
        // Attempts to parse the number as an integer. If the number is
        // larger than the maximum supported value of an integer then
        // we decode the number as a double.
        Location current = token.start_;
        const bool isNegative = *current == '-';
        if (isNegative) {
            ++current;
        }

        // We assume we can represent the largest and smallest integer types as
        // unsigned integers with separate sign. This is only true if they can fit
        // into an unsigned integer.
        static_assert(Value::maxLargestInt <= Value::maxLargestUInt,
            "Int must be smaller than UInt");

        // We need to convert minLargestInt into a positive number. The easiest way
        // to do this conversion is to assume our "threshold" value of minLargestInt
        // divided by 10 can fit in maxLargestInt when absolute valued. This should
        // be a safe assumption.
        static_assert(Value::minLargestInt <= -Value::maxLargestInt,
            "The absolute value of minLargestInt must be greater than or "
            "equal to maxLargestInt");
        static_assert(Value::minLargestInt / 10 >= -Value::maxLargestInt,
            "The absolute value of minLargestInt must be only 1 magnitude "
            "larger than maxLargest Int");

        static constexpr Value::LargestUInt positive_threshold =
            Value::maxLargestUInt / 10;
        static constexpr Value::UInt positive_last_digit = Value::maxLargestUInt % 10;

        // For the negative values, we have to be more careful. Since typically
        // -Value::minLargestInt will cause an overflow, we first divide by 10 and
        // then take the inverse. This assumes that minLargestInt is only a single
        // power of 10 different in magnitude, which we check above. For the last
        // digit, we take the modulus before negating for the same reason.
        static constexpr auto negative_threshold =
            Value::LargestUInt(-(Value::minLargestInt / 10));
        static constexpr auto negative_last_digit =
            Value::UInt(-(Value::minLargestInt % 10));

        const Value::LargestUInt threshold =
            isNegative ? negative_threshold : positive_threshold;
        const Value::UInt max_last_digit =
            isNegative ? negative_last_digit : positive_last_digit;

        Value::LargestUInt value = 0;
        while (current < token.end_) {
            Char c = *current++;
            if (c < '0' || c > '9')
                return decodeDouble(token, decoded);

            const auto digit(static_cast<Value::UInt>(c - '0'));
            if (value >= threshold) {
                // We've hit or exceeded the max value divided by 10 (rounded down). If
                // a) we've only just touched the limit, meaing value == threshold,
                // b) this is the last digit, or
                // c) it's small enough to fit in that rounding delta, we're okay.
                // Otherwise treat this number as a double to avoid overflow.
                if (value > threshold || current != token.end_ ||
                    digit > max_last_digit) {
                    return decodeDouble(token, decoded);
                }
            }
            value = value * 10 + digit;
        }

        if (isNegative) {
            // We use the same magnitude assumption here, just in case.
            const auto last_digit = static_cast<Value::UInt>(value % 10);
            decoded = -Value::LargestInt(value / 10) * 10 - last_digit;
        } else if (value <= Value::LargestUInt(Value::maxLargestInt)) {
            decoded = Value::LargestInt(value);
        } else {
            decoded = value;
        }

        return true;
    }

    bool OurReader::decodeDouble(Token& token) {
        Value decoded;
        if (!decodeDouble(token, decoded))
            return false;
        currentValue().swapPayload(decoded);
        currentValue().setOffsetStart(token.start_ - begin_);
        currentValue().setOffsetLimit(token.end_ - begin_);
        return true;
    }

    bool OurReader::decodeDouble(Token& token, Value& decoded) {
        double value = 0;
        const String buffer(token.start_, token.end_);
        IStringStream is(buffer);
        if (!(is >> value)) {
            return addError(
                "'" + String(token.start_, token.end_) + "' is not a number.", token);
        }
        decoded = value;
        return true;
    }

    bool OurReader::decodeString(Token& token) {
        String decoded_string;
        if (!decodeString(token, decoded_string))
            return false;
        Value decoded(decoded_string);
        currentValue().swapPayload(decoded);
        currentValue().setOffsetStart(token.start_ - begin_);
        currentValue().setOffsetLimit(token.end_ - begin_);
        return true;
    }

    bool OurReader::decodeString(Token& token, String& decoded) {
        decoded.reserve(static_cast<size_t>(token.end_ - token.start_ - 2));
        Location current = token.start_ + 1; // skip '"'
        Location end = token.end_ - 1;       // do not include '"'
        while (current != end) {
            Char c = *current++;
            if (c == '"')
                break;
            if (c == '\\') {
                if (current == end)
                    return addError("Empty escape sequence in string", token, current);
                Char escape = *current++;
                switch (escape) {
                case '"':
                    decoded += '"';
                    break;
                case '/':
                    decoded += '/';
                    break;
                case '\\':
                    decoded += '\\';
                    break;
                case 'b':
                    decoded += '\b';
                    break;
                case 'f':
                    decoded += '\f';
                    break;
                case 'n':
                    decoded += '\n';
                    break;
                case 'r':
                    decoded += '\r';
                    break;
                case 't':
                    decoded += '\t';
                    break;
                case 'u': {
                    unsigned int unicode;
                    if (!decodeUnicodeCodePoint(token, current, end, unicode))
                        return false;
                    decoded += codePointToUTF8(unicode);
                } break;
                default:
                    return addError("Bad escape sequence in string", token, current);
                }
            } else {
                decoded += c;
            }
        }
        return true;
    }

    bool OurReader::decodeUnicodeCodePoint(Token& token, Location& current,
        Location end, unsigned int& unicode) {

        if (!decodeUnicodeEscapeSequence(token, current, end, unicode))
            return false;
        if (unicode >= 0xD800 && unicode <= 0xDBFF) {
            // surrogate pairs
            if (end - current < 6)
                return addError(
                    "additional six characters expected to parse unicode surrogate pair.",
                    token, current);
            if (*(current++) == '\\' && *(current++) == 'u') {
                unsigned int surrogatePair;
                if (decodeUnicodeEscapeSequence(token, current, end, surrogatePair)) {
                    unicode = 0x10000 + ((unicode & 0x3FF) << 10) + (surrogatePair & 0x3FF);
                } else
                    return false;
            } else
                return addError("expecting another \\u token to begin the second half of "
                    "a unicode surrogate pair",
                    token, current);
        }
        return true;
    }

    bool OurReader::decodeUnicodeEscapeSequence(Token& token, Location& current,
        Location end,
        unsigned int& ret_unicode) {
        if (end - current < 4)
            return addError(
                "Bad unicode escape sequence in string: four digits expected.", token,
                current);
        int unicode = 0;
        for (int index = 0; index < 4; ++index) {
            Char c = *current++;
            unicode *= 16;
            if (c >= '0' && c <= '9')
                unicode += c - '0';
            else if (c >= 'a' && c <= 'f')
                unicode += c - 'a' + 10;
            else if (c >= 'A' && c <= 'F')
                unicode += c - 'A' + 10;
            else
                return addError(
                    "Bad unicode escape sequence in string: hexadecimal digit expected.",
                    token, current);
        }
        ret_unicode = static_cast<unsigned int>(unicode);
        return true;
    }

    bool OurReader::addError(const String& message, Token& token, Location extra) {
        ErrorInfo info;
        info.token_ = token;
        info.message_ = message;
        info.extra_ = extra;
        errors_.push_back(info);
        return false;
    }

    bool OurReader::recoverFromError(TokenType skipUntilToken) {
        size_t errorCount = errors_.size();
        Token skip;
        for (;;) {
            if (!readToken(skip))
                errors_.resize(errorCount); // discard errors caused by recovery
            if (skip.type_ == skipUntilToken || skip.type_ == tokenEndOfStream)
                break;
        }
        errors_.resize(errorCount);
        return false;
    }

    bool OurReader::addErrorAndRecover(const String& message, Token& token,
        TokenType skipUntilToken) {
        addError(message, token);
        return recoverFromError(skipUntilToken);
    }

    Value& OurReader::currentValue() { return *(nodes_.top()); }

    OurReader::Char OurReader::getNextChar() {
        if (current_ == end_)
            return 0;
        return *current_++;
    }

    void OurReader::getLocationLineAndColumn(Location location, int& line,
        int& column) const {
        Location current = begin_;
        Location lastLineStart = current;
        line = 0;
        while (current < location && current != end_) {
            Char c = *current++;
            if (c == '\r') {
                if (*current == '\n')
                    ++current;
                lastLineStart = current;
                ++line;
            } else if (c == '\n') {
                lastLineStart = current;
                ++line;
            }
        }
        // column & line start at 1
        column = int(location - lastLineStart) + 1;
        ++line;
    }

    String OurReader::getLocationLineAndColumn(Location location) const {
        int line, column;
        getLocationLineAndColumn(location, line, column);
        char buffer[18 + 16 + 16 + 1];
        jsoncpp_snprintf(buffer, sizeof(buffer), "Line %d, Column %d", line, column);
        return buffer;
    }

    String OurReader::getFormattedErrorMessages() const {
        String formattedMessage;
        for (const auto& error : errors_) {
            formattedMessage +=
                "* " + getLocationLineAndColumn(error.token_.start_) + "\n";
            formattedMessage += "  " + error.message_ + "\n";
            if (error.extra_)
                formattedMessage +=
                "See " + getLocationLineAndColumn(error.extra_) + " for detail.\n";
        }
        return formattedMessage;
    }

    std::vector<OurReader::StructuredError> OurReader::getStructuredErrors() const {
        std::vector<OurReader::StructuredError> allErrors;
        for (const auto& error : errors_) {
            OurReader::StructuredError structured;
            structured.offset_start = error.token_.start_ - begin_;
            structured.offset_limit = error.token_.end_ - begin_;
            structured.message = error.message_;
            allErrors.push_back(structured);
        }
        return allErrors;
    }

    class OurCharReader : public CharReader {
        bool const collectComments_;
        OurReader reader_;

    public:
        OurCharReader(bool collectComments, OurFeatures const& features)
            : collectComments_(collectComments), reader_(features) {}
        bool parse(char const* beginDoc, char const* endDoc, Value* root,
            String* errs) override {
            bool ok = reader_.parse(beginDoc, endDoc, *root, collectComments_);
            if (errs) {
                *errs = reader_.getFormattedErrorMessages();
            }
            return ok;
        }
    };

    CharReaderBuilder::CharReaderBuilder() { setDefaults(&settings_); }
    CharReaderBuilder::~CharReaderBuilder() = default;
    CharReader* CharReaderBuilder::newCharReader() const {
        bool collectComments = settings_["collectComments"].asBool();
        OurFeatures features = OurFeatures::all();
        features.allowComments_ = settings_["allowComments"].asBool();
        features.allowTrailingCommas_ = settings_["allowTrailingCommas"].asBool();
        features.strictRoot_ = settings_["strictRoot"].asBool();
        features.allowDroppedNullPlaceholders_ =
            settings_["allowDroppedNullPlaceholders"].asBool();
        features.allowNumericKeys_ = settings_["allowNumericKeys"].asBool();
        features.allowSingleQuotes_ = settings_["allowSingleQuotes"].asBool();

        // Stack limit is always a size_t, so we get this as an unsigned int
        // regardless of it we have 64-bit integer support enabled.
        features.stackLimit_ = static_cast<size_t>(settings_["stackLimit"].asUInt());
        features.failIfExtra_ = settings_["failIfExtra"].asBool();
        features.rejectDupKeys_ = settings_["rejectDupKeys"].asBool();
        features.allowSpecialFloats_ = settings_["allowSpecialFloats"].asBool();
        return new OurCharReader(collectComments, features);
    }
    static void getValidReaderKeys(std::set<String>* valid_keys) {
        valid_keys->clear();
        valid_keys->insert("collectComments");
        valid_keys->insert("allowComments");
        valid_keys->insert("allowTrailingCommas");
        valid_keys->insert("strictRoot");
        valid_keys->insert("allowDroppedNullPlaceholders");
        valid_keys->insert("allowNumericKeys");
        valid_keys->insert("allowSingleQuotes");
        valid_keys->insert("stackLimit");
        valid_keys->insert("failIfExtra");
        valid_keys->insert("rejectDupKeys");
        valid_keys->insert("allowSpecialFloats");
    }
    bool CharReaderBuilder::validate(Json::Value* invalid) const {
        Json::Value my_invalid;
        if (!invalid)
            invalid = &my_invalid; // so we do not need to test for NULL
        Json::Value& inv = *invalid;
        std::set<String> valid_keys;
        getValidReaderKeys(&valid_keys);
        Value::Members keys = settings_.getMemberNames();
        size_t n = keys.size();
        for (size_t i = 0; i < n; ++i) {
            String const& key = keys[i];
            if (valid_keys.find(key) == valid_keys.end()) {
                inv[key] = settings_[key];
            }
        }
        return inv.empty();
    }
    Value& CharReaderBuilder::operator[](const String& key) {
        return settings_[key];
    }
    // static
    void CharReaderBuilder::strictMode(Json::Value* settings) {
        //! [CharReaderBuilderStrictMode]
        (*settings)["allowComments"] = false;
        (*settings)["allowTrailingCommas"] = false;
        (*settings)["strictRoot"] = true;
        (*settings)["allowDroppedNullPlaceholders"] = false;
        (*settings)["allowNumericKeys"] = false;
        (*settings)["allowSingleQuotes"] = false;
        (*settings)["stackLimit"] = 1000;
        (*settings)["failIfExtra"] = true;
        (*settings)["rejectDupKeys"] = true;
        (*settings)["allowSpecialFloats"] = false;
        //! [CharReaderBuilderStrictMode]
    }
    // static
    void CharReaderBuilder::setDefaults(Json::Value* settings) {
        //! [CharReaderBuilderDefaults]
        (*settings)["collectComments"] = true;
        (*settings)["allowComments"] = true;
        (*settings)["allowTrailingCommas"] = true;
        (*settings)["strictRoot"] = false;
        (*settings)["allowDroppedNullPlaceholders"] = false;
        (*settings)["allowNumericKeys"] = false;
        (*settings)["allowSingleQuotes"] = false;
        (*settings)["stackLimit"] = 1000;
        (*settings)["failIfExtra"] = false;
        (*settings)["rejectDupKeys"] = false;
        (*settings)["allowSpecialFloats"] = false;
        //! [CharReaderBuilderDefaults]
    }

    //////////////////////////////////
    // global functions

    bool parseFromStream(CharReader::Factory const& fact, IStream& sin, Value* root,
        String* errs) {
        OStringStream ssin;
        ssin << sin.rdbuf();
        String doc = ssin.str();
        char const* begin = doc.data();
        char const* end = begin + doc.size();
        // Note that we do not actually need a null-terminator.
        CharReaderPtr const reader(fact.newCharReader());
        return reader->parse(begin, end, root, errs);
    }

    IStream& operator>>(IStream& sin, Value& root) {
        CharReaderBuilder b;
        String errs;
        bool ok = parseFromStream(b, sin, &root, &errs);
        if (!ok) {
            throwRuntimeError(errs);
        }
        return sin;
    }

} // namespace Json

// //////////////////////////////////////////////////////////////////////
// End of content of file: src/lib_json/json_reader.cpp
// //////////////////////////////////////////////////////////////////////






// //////////////////////////////////////////////////////////////////////
// Beginning of content of file: src/lib_json/json_valueiterator.inl
// //////////////////////////////////////////////////////////////////////

// Copyright 2007-2010 Baptiste Lepilleur and The JsonCpp Authors
// Distributed under MIT license, or public domain if desired and
// recognized in your jurisdiction.
// See file LICENSE for detail or copy at http://jsoncpp.sourceforge.net/LICENSE

// included by json_value.cpp

namespace Json {

    // //////////////////////////////////////////////////////////////////
    // //////////////////////////////////////////////////////////////////
    // //////////////////////////////////////////////////////////////////
    // class ValueIteratorBase
    // //////////////////////////////////////////////////////////////////
    // //////////////////////////////////////////////////////////////////
    // //////////////////////////////////////////////////////////////////

    ValueIteratorBase::ValueIteratorBase() : current_() {}

    ValueIteratorBase::ValueIteratorBase(
        const Value::ObjectValues::iterator& current)
        : current_(current), isNull_(false) {}

    Value& ValueIteratorBase::deref() { return current_->second; }
    const Value& ValueIteratorBase::deref() const { return current_->second; }

    void ValueIteratorBase::increment() { ++current_; }

    void ValueIteratorBase::decrement() { --current_; }

    ValueIteratorBase::difference_type
        ValueIteratorBase::computeDistance(const SelfType& other) const {
        // Iterator for null value are initialized using the default
        // constructor, which initialize current_ to the default
        // std::map::iterator. As begin() and end() are two instance
        // of the default std::map::iterator, they can not be compared.
        // To allow this, we handle this comparison specifically.
        if (isNull_ && other.isNull_) {
            return 0;
        }

        // Usage of std::distance is not portable (does not compile with Sun Studio 12
        // RogueWave STL,
        // which is the one used by default).
        // Using a portable hand-made version for non random iterator instead:
        //   return difference_type( std::distance( current_, other.current_ ) );
        difference_type myDistance = 0;
        for (Value::ObjectValues::iterator it = current_; it != other.current_;
            ++it) {
            ++myDistance;
        }
        return myDistance;
    }

    bool ValueIteratorBase::isEqual(const SelfType& other) const {
        if (isNull_) {
            return other.isNull_;
        }
        return current_ == other.current_;
    }

    void ValueIteratorBase::copy(const SelfType& other) {
        current_ = other.current_;
        isNull_ = other.isNull_;
    }

    Value ValueIteratorBase::key() const {
        const Value::CZString czstring = (*current_).first;
        if (czstring.data()) {
            if (czstring.isStaticString())
                return Value(StaticString(czstring.data()));
            return Value(czstring.data(), czstring.data() + czstring.length());
        }
        return Value(czstring.index());
    }

    UInt ValueIteratorBase::index() const {
        const Value::CZString czstring = (*current_).first;
        if (!czstring.data())
            return czstring.index();
        return Value::UInt(-1);
    }

    String ValueIteratorBase::name() const {
        char const* keey;
        char const* end;
        keey = memberName(&end);
        if (!keey)
            return String();
        return String(keey, end);
    }

    char const* ValueIteratorBase::memberName() const {
        const char* cname = (*current_).first.data();
        return cname ? cname : "";
    }

    char const* ValueIteratorBase::memberName(char const** end) const {
        const char* cname = (*current_).first.data();
        if (!cname) {
            *end = nullptr;
            return nullptr;
        }
        *end = cname + (*current_).first.length();
        return cname;
    }

    // //////////////////////////////////////////////////////////////////
    // //////////////////////////////////////////////////////////////////
    // //////////////////////////////////////////////////////////////////
    // class ValueConstIterator
    // //////////////////////////////////////////////////////////////////
    // //////////////////////////////////////////////////////////////////
    // //////////////////////////////////////////////////////////////////

    ValueConstIterator::ValueConstIterator() = default;

    ValueConstIterator::ValueConstIterator(
        const Value::ObjectValues::iterator& current)
        : ValueIteratorBase(current) {}

    ValueConstIterator::ValueConstIterator(ValueIterator const& other)
        : ValueIteratorBase(other) {}

    ValueConstIterator& ValueConstIterator::
        operator=(const ValueIteratorBase& other) {
        copy(other);
        return *this;
    }

    // //////////////////////////////////////////////////////////////////
    // //////////////////////////////////////////////////////////////////
    // //////////////////////////////////////////////////////////////////
    // class ValueIterator
    // //////////////////////////////////////////////////////////////////
    // //////////////////////////////////////////////////////////////////
    // //////////////////////////////////////////////////////////////////

    ValueIterator::ValueIterator() = default;

    ValueIterator::ValueIterator(const Value::ObjectValues::iterator& current)
        : ValueIteratorBase(current) {}

    ValueIterator::ValueIterator(const ValueConstIterator& other)
        : ValueIteratorBase(other) {
        throwRuntimeError("ConstIterator to Iterator should never be allowed.");
    }

    ValueIterator::ValueIterator(const ValueIterator& other) = default;

    ValueIterator& ValueIterator::operator=(const SelfType& other) {
        copy(other);
        return *this;
    }

} // namespace Json

// //////////////////////////////////////////////////////////////////////
// End of content of file: src/lib_json/json_valueiterator.inl
// //////////////////////////////////////////////////////////////////////






// //////////////////////////////////////////////////////////////////////
// Beginning of content of file: src/lib_json/json_value.cpp
// //////////////////////////////////////////////////////////////////////

// Copyright 2011 Baptiste Lepilleur and The JsonCpp Authors
// Distributed under MIT license, or public domain if desired and
// recognized in your jurisdiction.
// See file LICENSE for detail or copy at http://jsoncpp.sourceforge.net/LICENSE

#if !defined(JSON_IS_AMALGAMATION)
#include <json/assertions.h>
#include <json/value.h>
#include <json/writer.h>
#endif // if !defined(JSON_IS_AMALGAMATION)
#include <algorithm>
#include <cassert>
#include <cmath>
#include <cstddef>
#include <cstring>
#include <sstream>
#include <utility>

// Provide implementation equivalent of std::snprintf for older _MSC compilers
#if defined(_MSC_VER) && _MSC_VER < 1900
#include <stdarg.h>
static int msvc_pre1900_c99_vsnprintf(char* outBuf, size_t size,
    const char* format, va_list ap) {
    int count = -1;
    if (size != 0)
        count = _vsnprintf_s(outBuf, size, _TRUNCATE, format, ap);
    if (count == -1)
        count = _vscprintf(format, ap);
    return count;
}

int JSON_API msvc_pre1900_c99_snprintf(char* outBuf, size_t size,
    const char* format, ...) {
    va_list ap;
    va_start(ap, format);
    const int count = msvc_pre1900_c99_vsnprintf(outBuf, size, format, ap);
    va_end(ap);
    return count;
}
#endif

// Disable warning C4702 : unreachable code
#if defined(_MSC_VER)
#pragma warning(disable : 4702)
#endif

#define JSON_ASSERT_UNREACHABLE assert(false)

namespace Json {
    template <typename T>
    static std::unique_ptr<T> cloneUnique(const std::unique_ptr<T>& p) {
        std::unique_ptr<T> r;
        if (p) {
            r = std::unique_ptr<T>(new T(*p));
        }
        return r;
    }

    // This is a walkaround to avoid the static initialization of Value::null.
    // kNull must be word-aligned to avoid crashing on ARM.  We use an alignment of
    // 8 (instead of 4) as a bit of future-proofing.
#if defined(__ARMEL__)
#define ALIGNAS(byte_alignment) __attribute__((aligned(byte_alignment)))
#else
#define ALIGNAS(byte_alignment)
#endif

// static
    Value const& Value::nullSingleton() {
        static Value const nullStatic;
        return nullStatic;
    }

#if JSON_USE_NULLREF
    // for backwards compatibility, we'll leave these global references around, but
    // DO NOT use them in JSONCPP library code any more!
    // static
    Value const& Value::null = Value::nullSingleton();

    // static
    Value const& Value::nullRef = Value::nullSingleton();
#endif

#if !defined(JSON_USE_INT64_DOUBLE_CONVERSION)
    template <typename T, typename U>
    static inline bool InRange(double d, T min, U max) {
        // The casts can lose precision, but we are looking only for
        // an approximate range. Might fail on edge cases though. ~cdunn
        return d >= static_cast<double>(min) && d <= static_cast<double>(max);
    }
#else  // if !defined(JSON_USE_INT64_DOUBLE_CONVERSION)
    static inline double integerToDouble(Json::UInt64 value) {
        return static_cast<double>(Int64(value / 2)) * 2.0 +
            static_cast<double>(Int64(value & 1));
    }

    template <typename T> static inline double integerToDouble(T value) {
        return static_cast<double>(value);
    }

    template <typename T, typename U>
    static inline bool InRange(double d, T min, U max) {
        return d >= integerToDouble(min) && d <= integerToDouble(max);
    }
#endif // if !defined(JSON_USE_INT64_DOUBLE_CONVERSION)

    /** Duplicates the specified string value.
     * @param value Pointer to the string to duplicate. Must be zero-terminated if
     *              length is "unknown".
     * @param length Length of the value. if equals to unknown, then it will be
     *               computed using strlen(value).
     * @return Pointer on the duplicate instance of string.
     */
    static inline char* duplicateStringValue(const char* value, size_t length) {
        // Avoid an integer overflow in the call to malloc below by limiting length
        // to a sane value.
        if (length >= static_cast<size_t>(Value::maxInt))
            length = Value::maxInt - 1;

        auto newString = static_cast<char*>(malloc(length + 1));
        if (newString == nullptr) {
            throwRuntimeError("in Json::Value::duplicateStringValue(): "
                "Failed to allocate string value buffer");
        }
        memcpy(newString, value, length);
        newString[length] = 0;
        return newString;
    }

    /* Record the length as a prefix.
     */
    static inline char* duplicateAndPrefixStringValue(const char* value,
        unsigned int length) {
        // Avoid an integer overflow in the call to malloc below by limiting length
        // to a sane value.
        JSON_ASSERT_MESSAGE(length <= static_cast<unsigned>(Value::maxInt) -
            sizeof(unsigned) - 1U,
            "in Json::Value::duplicateAndPrefixStringValue(): "
            "length too big for prefixing");
        size_t actualLength = sizeof(length) + length + 1;
        auto newString = static_cast<char*>(malloc(actualLength));
        if (newString == nullptr) {
            throwRuntimeError("in Json::Value::duplicateAndPrefixStringValue(): "
                "Failed to allocate string value buffer");
        }
        *reinterpret_cast<unsigned*>(newString) = length;
        memcpy(newString + sizeof(unsigned), value, length);
        newString[actualLength - 1U] =
            0; // to avoid buffer over-run accidents by users later
        return newString;
    }
    inline static void decodePrefixedString(bool isPrefixed, char const* prefixed,
        unsigned* length, char const** value) {
        if (!isPrefixed) {
            *length = static_cast<unsigned>(strlen(prefixed));
            *value = prefixed;
        } else {
            *length = *reinterpret_cast<unsigned const*>(prefixed);
            *value = prefixed + sizeof(unsigned);
        }
    }
    /** Free the string duplicated by
     * duplicateStringValue()/duplicateAndPrefixStringValue().
     */
#if JSONCPP_USING_SECURE_MEMORY
    static inline void releasePrefixedStringValue(char* value) {
        unsigned length = 0;
        char const* valueDecoded;
        decodePrefixedString(true, value, &length, &valueDecoded);
        size_t const size = sizeof(unsigned) + length + 1U;
        memset(value, 0, size);
        free(value);
    }
    static inline void releaseStringValue(char* value, unsigned length) {
        // length==0 => we allocated the strings memory
        size_t size = (length == 0) ? strlen(value) : length;
        memset(value, 0, size);
        free(value);
    }
#else  // !JSONCPP_USING_SECURE_MEMORY
    static inline void releasePrefixedStringValue(char* value) { free(value); }
    static inline void releaseStringValue(char* value, unsigned) { free(value); }
#endif // JSONCPP_USING_SECURE_MEMORY

} // namespace Json

// //////////////////////////////////////////////////////////////////
// //////////////////////////////////////////////////////////////////
// //////////////////////////////////////////////////////////////////
// ValueInternals...
// //////////////////////////////////////////////////////////////////
// //////////////////////////////////////////////////////////////////
// //////////////////////////////////////////////////////////////////
#if !defined(JSON_IS_AMALGAMATION)

#include "json_valueiterator.inl"
#endif // if !defined(JSON_IS_AMALGAMATION)

namespace Json {

#if JSON_USE_EXCEPTION
    Exception::Exception(String msg) : msg_(std::move(msg)) {}
    Exception::~Exception() JSONCPP_NOEXCEPT = default;
    char const* Exception::what() const JSONCPP_NOEXCEPT { return msg_.c_str(); }
    RuntimeError::RuntimeError(String const& msg) : Exception(msg) {}
    LogicError::LogicError(String const& msg) : Exception(msg) {}
    JSONCPP_NORETURN void throwRuntimeError(String const& msg) {
        throw RuntimeError(msg);
    }
    JSONCPP_NORETURN void throwLogicError(String const& msg) {
        throw LogicError(msg);
    }
#else // !JSON_USE_EXCEPTION
    JSONCPP_NORETURN void throwRuntimeError(String const& msg) { abort(); }
    JSONCPP_NORETURN void throwLogicError(String const& msg) { abort(); }
#endif

    // //////////////////////////////////////////////////////////////////
    // //////////////////////////////////////////////////////////////////
    // //////////////////////////////////////////////////////////////////
    // class Value::CZString
    // //////////////////////////////////////////////////////////////////
    // //////////////////////////////////////////////////////////////////
    // //////////////////////////////////////////////////////////////////

    // Notes: policy_ indicates if the string was allocated when
    // a string is stored.

    Value::CZString::CZString(ArrayIndex index) : cstr_(nullptr), index_(index) {}

    Value::CZString::CZString(char const* str, unsigned length,
        DuplicationPolicy allocate)
        : cstr_(str) {
        // allocate != duplicate
        storage_.policy_ = allocate & 0x3;
        storage_.length_ = length & 0x3FFFFFFF;
    }

    Value::CZString::CZString(const CZString& other) {
        cstr_ = (other.storage_.policy_ != noDuplication && other.cstr_ != nullptr
            ? duplicateStringValue(other.cstr_, other.storage_.length_)
            : other.cstr_);
        storage_.policy_ =
            static_cast<unsigned>(
                other.cstr_
                ? (static_cast<DuplicationPolicy>(other.storage_.policy_) ==
                    noDuplication
                    ? noDuplication
                    : duplicate)
                : static_cast<DuplicationPolicy>(other.storage_.policy_)) &
            3U;
        storage_.length_ = other.storage_.length_;
    }

    Value::CZString::CZString(CZString&& other)
        : cstr_(other.cstr_), index_(other.index_) {
        other.cstr_ = nullptr;
    }

    Value::CZString::~CZString() {
        if (cstr_ && storage_.policy_ == duplicate) {
            releaseStringValue(const_cast<char*>(cstr_),
                storage_.length_ + 1U); // +1 for null terminating
                                        // character for sake of
                                        // completeness but not actually
                                        // necessary
        }
    }

    void Value::CZString::swap(CZString& other) {
        std::swap(cstr_, other.cstr_);
        std::swap(index_, other.index_);
    }

    Value::CZString& Value::CZString::operator=(const CZString& other) {
        cstr_ = other.cstr_;
        index_ = other.index_;
        return *this;
    }

    Value::CZString& Value::CZString::operator=(CZString&& other) {
        cstr_ = other.cstr_;
        index_ = other.index_;
        other.cstr_ = nullptr;
        return *this;
    }

    bool Value::CZString::operator<(const CZString& other) const {
        if (!cstr_)
            return index_ < other.index_;
        // return strcmp(cstr_, other.cstr_) < 0;
        // Assume both are strings.
        unsigned this_len = this->storage_.length_;
        unsigned other_len = other.storage_.length_;
        unsigned min_len = std::min<unsigned>(this_len, other_len);
        JSON_ASSERT(this->cstr_ && other.cstr_);
        int comp = memcmp(this->cstr_, other.cstr_, min_len);
        if (comp < 0)
            return true;
        if (comp > 0)
            return false;
        return (this_len < other_len);
    }

    bool Value::CZString::operator==(const CZString& other) const {
        if (!cstr_)
            return index_ == other.index_;
        // return strcmp(cstr_, other.cstr_) == 0;
        // Assume both are strings.
        unsigned this_len = this->storage_.length_;
        unsigned other_len = other.storage_.length_;
        if (this_len != other_len)
            return false;
        JSON_ASSERT(this->cstr_ && other.cstr_);
        int comp = memcmp(this->cstr_, other.cstr_, this_len);
        return comp == 0;
    }

    ArrayIndex Value::CZString::index() const { return index_; }

    // const char* Value::CZString::c_str() const { return cstr_; }
    const char* Value::CZString::data() const { return cstr_; }
    unsigned Value::CZString::length() const { return storage_.length_; }
    bool Value::CZString::isStaticString() const {
        return storage_.policy_ == noDuplication;
    }

    // //////////////////////////////////////////////////////////////////
    // //////////////////////////////////////////////////////////////////
    // //////////////////////////////////////////////////////////////////
    // class Value::Value
    // //////////////////////////////////////////////////////////////////
    // //////////////////////////////////////////////////////////////////
    // //////////////////////////////////////////////////////////////////

    /*! \internal Default constructor initialization must be equivalent to:
     * memset( this, 0, sizeof(Value) )
     * This optimization is used in ValueInternalMap fast allocator.
     */
    Value::Value(ValueType type) {
        static char const emptyString[] = "";
        initBasic(type);
        switch (type) {
        case nullValue:
            break;
        case intValue:
        case uintValue:
            value_.int_ = 0;
            break;
        case realValue:
            value_.real_ = 0.0;
            break;
        case stringValue:
            // allocated_ == false, so this is safe.
            value_.string_ = const_cast<char*>(static_cast<char const*>(emptyString));
            break;
        case arrayValue:
        case objectValue:
            value_.map_ = new ObjectValues();
            break;
        case booleanValue:
            value_.bool_ = false;
            break;
        default:
            JSON_ASSERT_UNREACHABLE;
        }
    }

    Value::Value(Int value) {
        initBasic(intValue);
        value_.int_ = value;
    }

    Value::Value(UInt value) {
        initBasic(uintValue);
        value_.uint_ = value;
    }
#if defined(JSON_HAS_INT64)
    Value::Value(Int64 value) {
        initBasic(intValue);
        value_.int_ = value;
    }
    Value::Value(UInt64 value) {
        initBasic(uintValue);
        value_.uint_ = value;
    }
#endif // defined(JSON_HAS_INT64)

    Value::Value(double value) {
        initBasic(realValue);
        value_.real_ = value;
    }

    Value::Value(const char* value) {
        initBasic(stringValue, true);
        JSON_ASSERT_MESSAGE(value != nullptr,
            "Null Value Passed to Value Constructor");
        value_.string_ = duplicateAndPrefixStringValue(
            value, static_cast<unsigned>(strlen(value)));
    }

    Value::Value(const char* begin, const char* end) {
        initBasic(stringValue, true);
        value_.string_ =
            duplicateAndPrefixStringValue(begin, static_cast<unsigned>(end - begin));
    }

    Value::Value(const String& value) {
        initBasic(stringValue, true);
        value_.string_ = duplicateAndPrefixStringValue(
            value.data(), static_cast<unsigned>(value.length()));
    }

    Value::Value(const StaticString& value) {
        initBasic(stringValue);
        value_.string_ = const_cast<char*>(value.c_str());
    }

    Value::Value(bool value) {
        initBasic(booleanValue);
        value_.bool_ = value;
    }

    Value::Value(const Value& other) {
        dupPayload(other);
        dupMeta(other);
    }

    Value::Value(Value&& other) {
        initBasic(nullValue);
        swap(other);
    }

    Value::~Value() {
        releasePayload();
        value_.uint_ = 0;
    }

    Value& Value::operator=(const Value& other) {
        Value(other).swap(*this);
        return *this;
    }

    Value& Value::operator=(Value&& other) {
        other.swap(*this);
        return *this;
    }

    void Value::swapPayload(Value& other) {
        std::swap(bits_, other.bits_);
        std::swap(value_, other.value_);
    }

    void Value::copyPayload(const Value& other) {
        releasePayload();
        dupPayload(other);
    }

    void Value::swap(Value& other) {
        swapPayload(other);
        std::swap(comments_, other.comments_);
        std::swap(start_, other.start_);
        std::swap(limit_, other.limit_);
    }

    void Value::copy(const Value& other) {
        copyPayload(other);
        dupMeta(other);
    }

    ValueType Value::type() const {
        return static_cast<ValueType>(bits_.value_type_);
    }

    int Value::compare(const Value& other) const {
        if (*this < other)
            return -1;
        if (*this > other)
            return 1;
        return 0;
    }

    bool Value::operator<(const Value& other) const {
        int typeDelta = type() - other.type();
        if (typeDelta)
            return typeDelta < 0;
        switch (type()) {
        case nullValue:
            return false;
        case intValue:
            return value_.int_ < other.value_.int_;
        case uintValue:
            return value_.uint_ < other.value_.uint_;
        case realValue:
            return value_.real_ < other.value_.real_;
        case booleanValue:
            return value_.bool_ < other.value_.bool_;
        case stringValue: {
            if ((value_.string_ == nullptr) || (other.value_.string_ == nullptr)) {
                return other.value_.string_ != nullptr;
            }
            unsigned this_len;
            unsigned other_len;
            char const* this_str;
            char const* other_str;
            decodePrefixedString(this->isAllocated(), this->value_.string_, &this_len,
                &this_str);
            decodePrefixedString(other.isAllocated(), other.value_.string_, &other_len,
                &other_str);
            unsigned min_len = std::min<unsigned>(this_len, other_len);
            JSON_ASSERT(this_str && other_str);
            int comp = memcmp(this_str, other_str, min_len);
            if (comp < 0)
                return true;
            if (comp > 0)
                return false;
            return (this_len < other_len);
        }
        case arrayValue:
        case objectValue: {
            auto thisSize = value_.map_->size();
            auto otherSize = other.value_.map_->size();
            if (thisSize != otherSize)
                return thisSize < otherSize;
            return (*value_.map_) < (*other.value_.map_);
        }
        default:
            JSON_ASSERT_UNREACHABLE;
        }
        return false; // unreachable
    }

    bool Value::operator<=(const Value& other) const { return !(other < *this); }

    bool Value::operator>=(const Value& other) const { return !(*this < other); }

    bool Value::operator>(const Value& other) const { return other < *this; }

    bool Value::operator==(const Value& other) const {
        if (type() != other.type())
            return false;
        switch (type()) {
        case nullValue:
            return true;
        case intValue:
            return value_.int_ == other.value_.int_;
        case uintValue:
            return value_.uint_ == other.value_.uint_;
        case realValue:
            return value_.real_ == other.value_.real_;
        case booleanValue:
            return value_.bool_ == other.value_.bool_;
        case stringValue: {
            if ((value_.string_ == nullptr) || (other.value_.string_ == nullptr)) {
                return (value_.string_ == other.value_.string_);
            }
            unsigned this_len;
            unsigned other_len;
            char const* this_str;
            char const* other_str;
            decodePrefixedString(this->isAllocated(), this->value_.string_, &this_len,
                &this_str);
            decodePrefixedString(other.isAllocated(), other.value_.string_, &other_len,
                &other_str);
            if (this_len != other_len)
                return false;
            JSON_ASSERT(this_str && other_str);
            int comp = memcmp(this_str, other_str, this_len);
            return comp == 0;
        }
        case arrayValue:
        case objectValue:
            return value_.map_->size() == other.value_.map_->size() &&
                (*value_.map_) == (*other.value_.map_);
        default:
            JSON_ASSERT_UNREACHABLE;
        }
        return false; // unreachable
    }

    bool Value::operator!=(const Value& other) const { return !(*this == other); }

    const char* Value::asCString() const {
        JSON_ASSERT_MESSAGE(type() == stringValue,
            "in Json::Value::asCString(): requires stringValue");
        if (value_.string_ == nullptr)
            return nullptr;
        unsigned this_len;
        char const* this_str;
        decodePrefixedString(this->isAllocated(), this->value_.string_, &this_len,
            &this_str);
        return this_str;
    }

#if JSONCPP_USING_SECURE_MEMORY
    unsigned Value::getCStringLength() const {
        JSON_ASSERT_MESSAGE(type() == stringValue,
            "in Json::Value::asCString(): requires stringValue");
        if (value_.string_ == 0)
            return 0;
        unsigned this_len;
        char const* this_str;
        decodePrefixedString(this->isAllocated(), this->value_.string_, &this_len,
            &this_str);
        return this_len;
    }
#endif

    bool Value::getString(char const** begin, char const** end) const {
        if (type() != stringValue)
            return false;
        if (value_.string_ == nullptr)
            return false;
        unsigned length;
        decodePrefixedString(this->isAllocated(), this->value_.string_, &length,
            begin);
        *end = *begin + length;
        return true;
    }

    String Value::asString() const {
        switch (type()) {
        case nullValue:
            return "";
        case stringValue: {
            if (value_.string_ == nullptr)
                return "";
            unsigned this_len;
            char const* this_str;
            decodePrefixedString(this->isAllocated(), this->value_.string_, &this_len,
                &this_str);
            return String(this_str, this_len);
        }
        case booleanValue:
            return value_.bool_ ? "true" : "false";
        case intValue:
            return valueToString(value_.int_);
        case uintValue:
            return valueToString(value_.uint_);
        case realValue:
            return valueToString(value_.real_);
        default:
            JSON_FAIL_MESSAGE("Type is not convertible to string");
        }
    }

    Value::Int Value::asInt() const {
        switch (type()) {
        case intValue:
            JSON_ASSERT_MESSAGE(isInt(), "LargestInt out of Int range");
            return Int(value_.int_);
        case uintValue:
            JSON_ASSERT_MESSAGE(isInt(), "LargestUInt out of Int range");
            return Int(value_.uint_);
        case realValue:
            JSON_ASSERT_MESSAGE(InRange(value_.real_, minInt, maxInt),
                "double out of Int range");
            return Int(value_.real_);
        case nullValue:
            return 0;
        case booleanValue:
            return value_.bool_ ? 1 : 0;
        default:
            break;
        }
        JSON_FAIL_MESSAGE("Value is not convertible to Int.");
    }

    Value::UInt Value::asUInt() const {
        switch (type()) {
        case intValue:
            JSON_ASSERT_MESSAGE(isUInt(), "LargestInt out of UInt range");
            return UInt(value_.int_);
        case uintValue:
            JSON_ASSERT_MESSAGE(isUInt(), "LargestUInt out of UInt range");
            return UInt(value_.uint_);
        case realValue:
            JSON_ASSERT_MESSAGE(InRange(value_.real_, 0, maxUInt),
                "double out of UInt range");
            return UInt(value_.real_);
        case nullValue:
            return 0;
        case booleanValue:
            return value_.bool_ ? 1 : 0;
        default:
            break;
        }
        JSON_FAIL_MESSAGE("Value is not convertible to UInt.");
    }

#if defined(JSON_HAS_INT64)

    Value::Int64 Value::asInt64() const {
        switch (type()) {
        case intValue:
            return Int64(value_.int_);
        case uintValue:
            JSON_ASSERT_MESSAGE(isInt64(), "LargestUInt out of Int64 range");
            return Int64(value_.uint_);
        case realValue:
            JSON_ASSERT_MESSAGE(InRange(value_.real_, minInt64, maxInt64),
                "double out of Int64 range");
            return Int64(value_.real_);
        case nullValue:
            return 0;
        case booleanValue:
            return value_.bool_ ? 1 : 0;
        default:
            break;
        }
        JSON_FAIL_MESSAGE("Value is not convertible to Int64.");
    }

    Value::UInt64 Value::asUInt64() const {
        switch (type()) {
        case intValue:
            JSON_ASSERT_MESSAGE(isUInt64(), "LargestInt out of UInt64 range");
            return UInt64(value_.int_);
        case uintValue:
            return UInt64(value_.uint_);
        case realValue:
            JSON_ASSERT_MESSAGE(InRange(value_.real_, 0, maxUInt64),
                "double out of UInt64 range");
            return UInt64(value_.real_);
        case nullValue:
            return 0;
        case booleanValue:
            return value_.bool_ ? 1 : 0;
        default:
            break;
        }
        JSON_FAIL_MESSAGE("Value is not convertible to UInt64.");
    }
#endif // if defined(JSON_HAS_INT64)

    LargestInt Value::asLargestInt() const {
#if defined(JSON_NO_INT64)
        return asInt();
#else
        return asInt64();
#endif
    }

    LargestUInt Value::asLargestUInt() const {
#if defined(JSON_NO_INT64)
        return asUInt();
#else
        return asUInt64();
#endif
    }

    double Value::asDouble() const {
        switch (type()) {
        case intValue:
            return static_cast<double>(value_.int_);
        case uintValue:
#if !defined(JSON_USE_INT64_DOUBLE_CONVERSION)
            return static_cast<double>(value_.uint_);
#else  // if !defined(JSON_USE_INT64_DOUBLE_CONVERSION)
            return integerToDouble(value_.uint_);
#endif // if !defined(JSON_USE_INT64_DOUBLE_CONVERSION)
        case realValue:
            return value_.real_;
        case nullValue:
            return 0.0;
        case booleanValue:
            return value_.bool_ ? 1.0 : 0.0;
        default:
            break;
        }
        JSON_FAIL_MESSAGE("Value is not convertible to double.");
    }

    float Value::asFloat() const {
        switch (type()) {
        case intValue:
            return static_cast<float>(value_.int_);
        case uintValue:
#if !defined(JSON_USE_INT64_DOUBLE_CONVERSION)
            return static_cast<float>(value_.uint_);
#else  // if !defined(JSON_USE_INT64_DOUBLE_CONVERSION)
            // This can fail (silently?) if the value is bigger than MAX_FLOAT.
            return static_cast<float>(integerToDouble(value_.uint_));
#endif // if !defined(JSON_USE_INT64_DOUBLE_CONVERSION)
        case realValue:
            return static_cast<float>(value_.real_);
        case nullValue:
            return 0.0;
        case booleanValue:
            return value_.bool_ ? 1.0F : 0.0F;
        default:
            break;
        }
        JSON_FAIL_MESSAGE("Value is not convertible to float.");
    }

    bool Value::asBool() const {
        switch (type()) {
        case booleanValue:
            return value_.bool_;
        case nullValue:
            return false;
        case intValue:
            return value_.int_ != 0;
        case uintValue:
            return value_.uint_ != 0;
        case realValue: {
            // According to JavaScript language zero or NaN is regarded as false
            const auto value_classification = std::fpclassify(value_.real_);
            return value_classification != FP_ZERO && value_classification != FP_NAN;
        }
        default:
            break;
        }
        JSON_FAIL_MESSAGE("Value is not convertible to bool.");
    }

    bool Value::isConvertibleTo(ValueType other) const {
        switch (other) {
        case nullValue:
            return (isNumeric() && asDouble() == 0.0) ||
                (type() == booleanValue && !value_.bool_) ||
                (type() == stringValue && asString().empty()) ||
                (type() == arrayValue && value_.map_->empty()) ||
                (type() == objectValue && value_.map_->empty()) ||
                type() == nullValue;
        case intValue:
            return isInt() ||
                (type() == realValue && InRange(value_.real_, minInt, maxInt)) ||
                type() == booleanValue || type() == nullValue;
        case uintValue:
            return isUInt() ||
                (type() == realValue && InRange(value_.real_, 0, maxUInt)) ||
                type() == booleanValue || type() == nullValue;
        case realValue:
            return isNumeric() || type() == booleanValue || type() == nullValue;
        case booleanValue:
            return isNumeric() || type() == booleanValue || type() == nullValue;
        case stringValue:
            return isNumeric() || type() == booleanValue || type() == stringValue ||
                type() == nullValue;
        case arrayValue:
            return type() == arrayValue || type() == nullValue;
        case objectValue:
            return type() == objectValue || type() == nullValue;
        }
        JSON_ASSERT_UNREACHABLE;
        return false;
    }

    /// Number of values in array or object
    ArrayIndex Value::size() const {
        switch (type()) {
        case nullValue:
        case intValue:
        case uintValue:
        case realValue:
        case booleanValue:
        case stringValue:
            return 0;
        case arrayValue: // size of the array is highest index + 1
            if (!value_.map_->empty()) {
                ObjectValues::const_iterator itLast = value_.map_->end();
                --itLast;
                return (*itLast).first.index() + 1;
            }
            return 0;
        case objectValue:
            return ArrayIndex(value_.map_->size());
        }
        JSON_ASSERT_UNREACHABLE;
        return 0; // unreachable;
    }

    bool Value::empty() const {
        if (isNull() || isArray() || isObject())
            return size() == 0U;
        return false;
    }

    Value::operator bool() const { return !isNull(); }

    void Value::clear() {
        JSON_ASSERT_MESSAGE(type() == nullValue || type() == arrayValue ||
            type() == objectValue,
            "in Json::Value::clear(): requires complex value");
        start_ = 0;
        limit_ = 0;
        switch (type()) {
        case arrayValue:
        case objectValue:
            value_.map_->clear();
            break;
        default:
            break;
        }
    }

    void Value::resize(ArrayIndex newSize) {
        JSON_ASSERT_MESSAGE(type() == nullValue || type() == arrayValue,
            "in Json::Value::resize(): requires arrayValue");
        if (type() == nullValue)
            *this = Value(arrayValue);
        ArrayIndex oldSize = size();
        if (newSize == 0)
            clear();
        else if (newSize > oldSize)
            this->operator[](newSize - 1);
        else {
            for (ArrayIndex index = newSize; index < oldSize; ++index) {
                value_.map_->erase(index);
            }
            JSON_ASSERT(size() == newSize);
        }
    }

    Value& Value::operator[](ArrayIndex index) {
        JSON_ASSERT_MESSAGE(
            type() == nullValue || type() == arrayValue,
            "in Json::Value::operator[](ArrayIndex): requires arrayValue");
        if (type() == nullValue)
            *this = Value(arrayValue);
        CZString key(index);
        auto it = value_.map_->lower_bound(key);
        if (it != value_.map_->end() && (*it).first == key)
            return (*it).second;

        ObjectValues::value_type defaultValue(key, nullSingleton());
        it = value_.map_->insert(it, defaultValue);
        return (*it).second;
    }

    Value& Value::operator[](int index) {
        JSON_ASSERT_MESSAGE(
            index >= 0,
            "in Json::Value::operator[](int index): index cannot be negative");
        return (*this)[ArrayIndex(index)];
    }

    const Value& Value::operator[](ArrayIndex index) const {
        JSON_ASSERT_MESSAGE(
            type() == nullValue || type() == arrayValue,
            "in Json::Value::operator[](ArrayIndex)const: requires arrayValue");
        if (type() == nullValue)
            return nullSingleton();
        CZString key(index);
        ObjectValues::const_iterator it = value_.map_->find(key);
        if (it == value_.map_->end())
            return nullSingleton();
        return (*it).second;
    }

    const Value& Value::operator[](int index) const {
        JSON_ASSERT_MESSAGE(
            index >= 0,
            "in Json::Value::operator[](int index) const: index cannot be negative");
        return (*this)[ArrayIndex(index)];
    }

    void Value::initBasic(ValueType type, bool allocated) {
        setType(type);
        setIsAllocated(allocated);
        comments_ = Comments{};
        start_ = 0;
        limit_ = 0;
    }

    void Value::dupPayload(const Value& other) {
        setType(other.type());
        setIsAllocated(false);
        switch (type()) {
        case nullValue:
        case intValue:
        case uintValue:
        case realValue:
        case booleanValue:
            value_ = other.value_;
            break;
        case stringValue:
            if (other.value_.string_ && other.isAllocated()) {
                unsigned len;
                char const* str;
                decodePrefixedString(other.isAllocated(), other.value_.string_, &len,
                    &str);
                value_.string_ = duplicateAndPrefixStringValue(str, len);
                setIsAllocated(true);
            } else {
                value_.string_ = other.value_.string_;
            }
            break;
        case arrayValue:
        case objectValue:
            value_.map_ = new ObjectValues(*other.value_.map_);
            break;
        default:
            JSON_ASSERT_UNREACHABLE;
        }
    }

    void Value::releasePayload() {
        switch (type()) {
        case nullValue:
        case intValue:
        case uintValue:
        case realValue:
        case booleanValue:
            break;
        case stringValue:
            if (isAllocated())
                releasePrefixedStringValue(value_.string_);
            break;
        case arrayValue:
        case objectValue:
            delete value_.map_;
            break;
        default:
            JSON_ASSERT_UNREACHABLE;
        }
    }

    void Value::dupMeta(const Value& other) {
        comments_ = other.comments_;
        start_ = other.start_;
        limit_ = other.limit_;
    }

    // Access an object value by name, create a null member if it does not exist.
    // @pre Type of '*this' is object or null.
    // @param key is null-terminated.
    Value& Value::resolveReference(const char* key) {
        JSON_ASSERT_MESSAGE(
            type() == nullValue || type() == objectValue,
            "in Json::Value::resolveReference(): requires objectValue");
        if (type() == nullValue)
            *this = Value(objectValue);
        CZString actualKey(key, static_cast<unsigned>(strlen(key)),
            CZString::noDuplication); // NOTE!
        auto it = value_.map_->lower_bound(actualKey);
        if (it != value_.map_->end() && (*it).first == actualKey)
            return (*it).second;

        ObjectValues::value_type defaultValue(actualKey, nullSingleton());
        it = value_.map_->insert(it, defaultValue);
        Value& value = (*it).second;
        return value;
    }

    // @param key is not null-terminated.
    Value& Value::resolveReference(char const* key, char const* end) {
        JSON_ASSERT_MESSAGE(
            type() == nullValue || type() == objectValue,
            "in Json::Value::resolveReference(key, end): requires objectValue");
        if (type() == nullValue)
            *this = Value(objectValue);
        CZString actualKey(key, static_cast<unsigned>(end - key),
            CZString::duplicateOnCopy);
        auto it = value_.map_->lower_bound(actualKey);
        if (it != value_.map_->end() && (*it).first == actualKey)
            return (*it).second;

        ObjectValues::value_type defaultValue(actualKey, nullSingleton());
        it = value_.map_->insert(it, defaultValue);
        Value& value = (*it).second;
        return value;
    }

    Value Value::get(ArrayIndex index, const Value& defaultValue) const {
        const Value* value = &((*this)[index]);
        return value == &nullSingleton() ? defaultValue : *value;
    }

    bool Value::isValidIndex(ArrayIndex index) const { return index < size(); }

    Value const* Value::find(char const* begin, char const* end) const {
        JSON_ASSERT_MESSAGE(type() == nullValue || type() == objectValue,
            "in Json::Value::find(begin, end): requires "
            "objectValue or nullValue");
        if (type() == nullValue)
            return nullptr;
        CZString actualKey(begin, static_cast<unsigned>(end - begin),
            CZString::noDuplication);
        ObjectValues::const_iterator it = value_.map_->find(actualKey);
        if (it == value_.map_->end())
            return nullptr;
        return &(*it).second;
    }
    Value* Value::demand(char const* begin, char const* end) {
        JSON_ASSERT_MESSAGE(type() == nullValue || type() == objectValue,
            "in Json::Value::demand(begin, end): requires "
            "objectValue or nullValue");
        return &resolveReference(begin, end);
    }
    const Value& Value::operator[](const char* key) const {
        Value const* found = find(key, key + strlen(key));
        if (!found)
            return nullSingleton();
        return *found;
    }
    Value const& Value::operator[](const String& key) const {
        Value const* found = find(key.data(), key.data() + key.length());
        if (!found)
            return nullSingleton();
        return *found;
    }

    Value& Value::operator[](const char* key) {
        return resolveReference(key, key + strlen(key));
    }

    Value& Value::operator[](const String& key) {
        return resolveReference(key.data(), key.data() + key.length());
    }

    Value& Value::operator[](const StaticString& key) {
        return resolveReference(key.c_str());
    }

    Value& Value::append(const Value& value) { return append(Value(value)); }

    Value& Value::append(Value&& value) {
        JSON_ASSERT_MESSAGE(type() == nullValue || type() == arrayValue,
            "in Json::Value::append: requires arrayValue");
        if (type() == nullValue) {
            *this = Value(arrayValue);
        }
        return this->value_.map_->emplace(size(), std::move(value)).first->second;
    }

    bool Value::insert(ArrayIndex index, const Value& newValue) {
        return insert(index, Value(newValue));
    }

    bool Value::insert(ArrayIndex index, Value&& newValue) {
        JSON_ASSERT_MESSAGE(type() == nullValue || type() == arrayValue,
            "in Json::Value::insert: requires arrayValue");
        ArrayIndex length = size();
        if (index > length) {
            return false;
        }
        for (ArrayIndex i = length; i > index; i--) {
            (*this)[i] = std::move((*this)[i - 1]);
        }
        (*this)[index] = std::move(newValue);
        return true;
    }

    Value Value::get(char const* begin, char const* end,
        Value const& defaultValue) const {
        Value const* found = find(begin, end);
        return !found ? defaultValue : *found;
    }
    Value Value::get(char const* key, Value const& defaultValue) const {
        return get(key, key + strlen(key), defaultValue);
    }
    Value Value::get(String const& key, Value const& defaultValue) const {
        return get(key.data(), key.data() + key.length(), defaultValue);
    }

    bool Value::removeMember(const char* begin, const char* end, Value* removed) {
        if (type() != objectValue) {
            return false;
        }
        CZString actualKey(begin, static_cast<unsigned>(end - begin),
            CZString::noDuplication);
        auto it = value_.map_->find(actualKey);
        if (it == value_.map_->end())
            return false;
        if (removed)
            *removed = std::move(it->second);
        value_.map_->erase(it);
        return true;
    }
    bool Value::removeMember(const char* key, Value* removed) {
        return removeMember(key, key + strlen(key), removed);
    }
    bool Value::removeMember(String const& key, Value* removed) {
        return removeMember(key.data(), key.data() + key.length(), removed);
    }
    void Value::removeMember(const char* key) {
        JSON_ASSERT_MESSAGE(type() == nullValue || type() == objectValue,
            "in Json::Value::removeMember(): requires objectValue");
        if (type() == nullValue)
            return;

        CZString actualKey(key, unsigned(strlen(key)), CZString::noDuplication);
        value_.map_->erase(actualKey);
    }
    void Value::removeMember(const String& key) { removeMember(key.c_str()); }

    bool Value::removeIndex(ArrayIndex index, Value* removed) {
        if (type() != arrayValue) {
            return false;
        }
        CZString key(index);
        auto it = value_.map_->find(key);
        if (it == value_.map_->end()) {
            return false;
        }
        if (removed)
            *removed = it->second;
        ArrayIndex oldSize = size();
        // shift left all items left, into the place of the "removed"
        for (ArrayIndex i = index; i < (oldSize - 1); ++i) {
            CZString keey(i);
            (*value_.map_)[keey] = (*this)[i + 1];
        }
        // erase the last one ("leftover")
        CZString keyLast(oldSize - 1);
        auto itLast = value_.map_->find(keyLast);
        value_.map_->erase(itLast);
        return true;
    }

    bool Value::isMember(char const* begin, char const* end) const {
        Value const* value = find(begin, end);
        return nullptr != value;
    }
    bool Value::isMember(char const* key) const {
        return isMember(key, key + strlen(key));
    }
    bool Value::isMember(String const& key) const {
        return isMember(key.data(), key.data() + key.length());
    }

    Value::Members Value::getMemberNames() const {
        JSON_ASSERT_MESSAGE(
            type() == nullValue || type() == objectValue,
            "in Json::Value::getMemberNames(), value must be objectValue");
        if (type() == nullValue)
            return Value::Members();
        Members members;
        members.reserve(value_.map_->size());
        ObjectValues::const_iterator it = value_.map_->begin();
        ObjectValues::const_iterator itEnd = value_.map_->end();
        for (; it != itEnd; ++it) {
            members.push_back(String((*it).first.data(), (*it).first.length()));
        }
        return members;
    }

    static bool IsIntegral(double d) {
        double integral_part;
        return modf(d, &integral_part) == 0.0;
    }

    bool Value::isNull() const { return type() == nullValue; }

    bool Value::isBool() const { return type() == booleanValue; }

    bool Value::isInt() const {
        switch (type()) {
        case intValue:
#if defined(JSON_HAS_INT64)
            return value_.int_ >= minInt && value_.int_ <= maxInt;
#else
            return true;
#endif
        case uintValue:
            return value_.uint_ <= UInt(maxInt);
        case realValue:
            return value_.real_ >= minInt && value_.real_ <= maxInt &&
                IsIntegral(value_.real_);
        default:
            break;
        }
        return false;
    }

    bool Value::isUInt() const {
        switch (type()) {
        case intValue:
#if defined(JSON_HAS_INT64)
            return value_.int_ >= 0 && LargestUInt(value_.int_) <= LargestUInt(maxUInt);
#else
            return value_.int_ >= 0;
#endif
        case uintValue:
#if defined(JSON_HAS_INT64)
            return value_.uint_ <= maxUInt;
#else
            return true;
#endif
        case realValue:
            return value_.real_ >= 0 && value_.real_ <= maxUInt &&
                IsIntegral(value_.real_);
        default:
            break;
        }
        return false;
    }

    bool Value::isInt64() const {
#if defined(JSON_HAS_INT64)
        switch (type()) {
        case intValue:
            return true;
        case uintValue:
            return value_.uint_ <= UInt64(maxInt64);
        case realValue:
            // Note that maxInt64 (= 2^63 - 1) is not exactly representable as a
            // double, so double(maxInt64) will be rounded up to 2^63. Therefore we
            // require the value to be strictly less than the limit.
            return value_.real_ >= double(minInt64) &&
                value_.real_ < double(maxInt64) && IsIntegral(value_.real_);
        default:
            break;
        }
#endif // JSON_HAS_INT64
        return false;
    }

    bool Value::isUInt64() const {
#if defined(JSON_HAS_INT64)
        switch (type()) {
        case intValue:
            return value_.int_ >= 0;
        case uintValue:
            return true;
        case realValue:
            // Note that maxUInt64 (= 2^64 - 1) is not exactly representable as a
            // double, so double(maxUInt64) will be rounded up to 2^64. Therefore we
            // require the value to be strictly less than the limit.
            return value_.real_ >= 0 && value_.real_ < maxUInt64AsDouble &&
                IsIntegral(value_.real_);
        default:
            break;
        }
#endif // JSON_HAS_INT64
        return false;
    }

    bool Value::isIntegral() const {
        switch (type()) {
        case intValue:
        case uintValue:
            return true;
        case realValue:
#if defined(JSON_HAS_INT64)
            // Note that maxUInt64 (= 2^64 - 1) is not exactly representable as a
            // double, so double(maxUInt64) will be rounded up to 2^64. Therefore we
            // require the value to be strictly less than the limit.
            return value_.real_ >= double(minInt64) &&
                value_.real_ < maxUInt64AsDouble && IsIntegral(value_.real_);
#else
            return value_.real_ >= minInt && value_.real_ <= maxUInt &&
                IsIntegral(value_.real_);
#endif // JSON_HAS_INT64
        default:
            break;
        }
        return false;
    }

    bool Value::isDouble() const {
        return type() == intValue || type() == uintValue || type() == realValue;
    }

    bool Value::isNumeric() const { return isDouble(); }

    bool Value::isString() const { return type() == stringValue; }

    bool Value::isArray() const { return type() == arrayValue; }

    bool Value::isObject() const { return type() == objectValue; }

    Value::Comments::Comments(const Comments& that)
        : ptr_{ cloneUnique(that.ptr_) } {}

    Value::Comments::Comments(Comments&& that) : ptr_{ std::move(that.ptr_) } {}

    Value::Comments& Value::Comments::operator=(const Comments& that) {
        ptr_ = cloneUnique(that.ptr_);
        return *this;
    }

    Value::Comments& Value::Comments::operator=(Comments&& that) {
        ptr_ = std::move(that.ptr_);
        return *this;
    }

    bool Value::Comments::has(CommentPlacement slot) const {
        return ptr_ && !(*ptr_)[slot].empty();
    }

    String Value::Comments::get(CommentPlacement slot) const {
        if (!ptr_)
            return {};
        return (*ptr_)[slot];
    }

    void Value::Comments::set(CommentPlacement slot, String comment) {
        if (!ptr_) {
            ptr_ = std::unique_ptr<Array>(new Array());
        }
        // check comments array boundry.
        if (slot < CommentPlacement::numberOfCommentPlacement) {
            (*ptr_)[slot] = std::move(comment);
        }
    }

    void Value::setComment(String comment, CommentPlacement placement) {
        if (!comment.empty() && (comment.back() == '\n')) {
            // Always discard trailing newline, to aid indentation.
            comment.pop_back();
        }
        JSON_ASSERT(!comment.empty());
        JSON_ASSERT_MESSAGE(
            comment[0] == '\0' || comment[0] == '/',
            "in Json::Value::setComment(): Comments must start with /");
        comments_.set(placement, std::move(comment));
    }

    bool Value::hasComment(CommentPlacement placement) const {
        return comments_.has(placement);
    }

    String Value::getComment(CommentPlacement placement) const {
        return comments_.get(placement);
    }

    void Value::setOffsetStart(ptrdiff_t start) { start_ = start; }

    void Value::setOffsetLimit(ptrdiff_t limit) { limit_ = limit; }

    ptrdiff_t Value::getOffsetStart() const { return start_; }

    ptrdiff_t Value::getOffsetLimit() const { return limit_; }

    String Value::toStyledString() const {
        StreamWriterBuilder builder;

        String out = this->hasComment(commentBefore) ? "\n" : "";
        out += Json::writeString(builder, *this);
        out += '\n';

        return out;
    }

    Value::const_iterator Value::begin() const {
        switch (type()) {
        case arrayValue:
        case objectValue:
            if (value_.map_)
                return const_iterator(value_.map_->begin());
            break;
        default:
            break;
        }
        return {};
    }

    Value::const_iterator Value::end() const {
        switch (type()) {
        case arrayValue:
        case objectValue:
            if (value_.map_)
                return const_iterator(value_.map_->end());
            break;
        default:
            break;
        }
        return {};
    }

    Value::iterator Value::begin() {
        switch (type()) {
        case arrayValue:
        case objectValue:
            if (value_.map_)
                return iterator(value_.map_->begin());
            break;
        default:
            break;
        }
        return iterator();
    }

    Value::iterator Value::end() {
        switch (type()) {
        case arrayValue:
        case objectValue:
            if (value_.map_)
                return iterator(value_.map_->end());
            break;
        default:
            break;
        }
        return iterator();
    }

    // class PathArgument
    // //////////////////////////////////////////////////////////////////

    PathArgument::PathArgument() = default;

    PathArgument::PathArgument(ArrayIndex index)
        : index_(index), kind_(kindIndex) {}

    PathArgument::PathArgument(const char* key) : key_(key), kind_(kindKey) {}

    PathArgument::PathArgument(String key) : key_(std::move(key)), kind_(kindKey) {}

    // class Path
    // //////////////////////////////////////////////////////////////////

    Path::Path(const String& path, const PathArgument& a1, const PathArgument& a2,
        const PathArgument& a3, const PathArgument& a4,
        const PathArgument& a5) {
        InArgs in;
        in.reserve(5);
        in.push_back(&a1);
        in.push_back(&a2);
        in.push_back(&a3);
        in.push_back(&a4);
        in.push_back(&a5);
        makePath(path, in);
    }

    void Path::makePath(const String& path, const InArgs& in) {
        const char* current = path.c_str();
        const char* end = current + path.length();
        auto itInArg = in.begin();
        while (current != end) {
            if (*current == '[') {
                ++current;
                if (*current == '%')
                    addPathInArg(path, in, itInArg, PathArgument::kindIndex);
                else {
                    ArrayIndex index = 0;
                    for (; current != end && *current >= '0' && *current <= '9'; ++current)
                        index = index * 10 + ArrayIndex(*current - '0');
                    args_.push_back(index);
                }
                if (current == end || *++current != ']')
                    invalidPath(path, int(current - path.c_str()));
            } else if (*current == '%') {
                addPathInArg(path, in, itInArg, PathArgument::kindKey);
                ++current;
            } else if (*current == '.' || *current == ']') {
                ++current;
            } else {
                const char* beginName = current;
                while (current != end && !strchr("[.", *current))
                    ++current;
                args_.push_back(String(beginName, current));
            }
        }
    }

    void Path::addPathInArg(const String& /*path*/, const InArgs& in,
        InArgs::const_iterator& itInArg,
        PathArgument::Kind kind) {
        if (itInArg == in.end()) {
            // Error: missing argument %d
        } else if ((*itInArg)->kind_ != kind) {
            // Error: bad argument type
        } else {
            args_.push_back(**itInArg++);
        }
    }

    void Path::invalidPath(const String& /*path*/, int /*location*/) {
        // Error: invalid path.
    }

    const Value& Path::resolve(const Value& root) const {
        const Value* node = &root;
        for (const auto& arg : args_) {
            if (arg.kind_ == PathArgument::kindIndex) {
                if (!node->isArray() || !node->isValidIndex(arg.index_)) {
                    // Error: unable to resolve path (array value expected at position... )
                    return Value::nullSingleton();
                }
                node = &((*node)[arg.index_]);
            } else if (arg.kind_ == PathArgument::kindKey) {
                if (!node->isObject()) {
                    // Error: unable to resolve path (object value expected at position...)
                    return Value::nullSingleton();
                }
                node = &((*node)[arg.key_]);
                if (node == &Value::nullSingleton()) {
                    // Error: unable to resolve path (object has no member named '' at
                    // position...)
                    return Value::nullSingleton();
                }
            }
        }
        return *node;
    }

    Value Path::resolve(const Value& root, const Value& defaultValue) const {
        const Value* node = &root;
        for (const auto& arg : args_) {
            if (arg.kind_ == PathArgument::kindIndex) {
                if (!node->isArray() || !node->isValidIndex(arg.index_))
                    return defaultValue;
                node = &((*node)[arg.index_]);
            } else if (arg.kind_ == PathArgument::kindKey) {
                if (!node->isObject())
                    return defaultValue;
                node = &((*node)[arg.key_]);
                if (node == &Value::nullSingleton())
                    return defaultValue;
            }
        }
        return *node;
    }

    Value& Path::make(Value& root) const {
        Value* node = &root;
        for (const auto& arg : args_) {
            if (arg.kind_ == PathArgument::kindIndex) {
                if (!node->isArray()) {
                    // Error: node is not an array at position ...
                }
                node = &((*node)[arg.index_]);
            } else if (arg.kind_ == PathArgument::kindKey) {
                if (!node->isObject()) {
                    // Error: node is not an object at position...
                }
                node = &((*node)[arg.key_]);
            }
        }
        return *node;
    }

} // namespace Json

// //////////////////////////////////////////////////////////////////////
// End of content of file: src/lib_json/json_value.cpp
// //////////////////////////////////////////////////////////////////////






// //////////////////////////////////////////////////////////////////////
// Beginning of content of file: src/lib_json/json_writer.cpp
// //////////////////////////////////////////////////////////////////////

// Copyright 2011 Baptiste Lepilleur and The JsonCpp Authors
// Distributed under MIT license, or public domain if desired and
// recognized in your jurisdiction.
// See file LICENSE for detail or copy at http://jsoncpp.sourceforge.net/LICENSE

#if !defined(JSON_IS_AMALGAMATION)
#include "json_tool.h"
#include <json/writer.h>
#endif // if !defined(JSON_IS_AMALGAMATION)
#include <cassert>
#include <cstring>
#include <iomanip>
#include <memory>
#include <set>
#include <sstream>
#include <utility>

#if __cplusplus >= 201103L
#include <cmath>
#include <cstdio>

#if !defined(isnan)
#define isnan std::isnan
#endif

#if !defined(isfinite)
#define isfinite std::isfinite
#endif

#else
#include <cmath>
#include <cstdio>

#if defined(_MSC_VER)
#if !defined(isnan)
#include <float.h>
#define isnan _isnan
#endif

#if !defined(isfinite)
#include <float.h>
#define isfinite _finite
#endif

#if !defined(_CRT_SECURE_CPP_OVERLOAD_STANDARD_NAMES)
#define _CRT_SECURE_CPP_OVERLOAD_STANDARD_NAMES 1
#endif //_CRT_SECURE_CPP_OVERLOAD_STANDARD_NAMES

#endif //_MSC_VER

#if defined(__sun) && defined(__SVR4) // Solaris
#if !defined(isfinite)
#include <ieeefp.h>
#define isfinite finite
#endif
#endif

#if defined(__hpux)
#if !defined(isfinite)
#if defined(__ia64) && !defined(finite)
#define isfinite(x)                                                            \
  ((sizeof(x) == sizeof(float) ? _Isfinitef(x) : _IsFinite(x)))
#endif
#endif
#endif

#if !defined(isnan)
// IEEE standard states that NaN values will not compare to themselves
#define isnan(x) (x != x)
#endif

#if !defined(__APPLE__)
#if !defined(isfinite)
#define isfinite finite
#endif
#endif
#endif

#if defined(_MSC_VER)
// Disable warning about strdup being deprecated.
#pragma warning(disable : 4996)
#endif

namespace Json {

#if __cplusplus >= 201103L || (defined(_CPPLIB_VER) && _CPPLIB_VER >= 520)
    using StreamWriterPtr = std::unique_ptr<StreamWriter>;
#else
    using StreamWriterPtr = std::auto_ptr<StreamWriter>;
#endif

    String valueToString(LargestInt value) {
        UIntToStringBuffer buffer;
        char* current = buffer + sizeof(buffer);
        if (value == Value::minLargestInt) {
            uintToString(LargestUInt(Value::maxLargestInt) + 1, current);
            *--current = '-';
        } else if (value < 0) {
            uintToString(LargestUInt(-value), current);
            *--current = '-';
        } else {
            uintToString(LargestUInt(value), current);
        }
        assert(current >= buffer);
        return current;
    }

    String valueToString(LargestUInt value) {
        UIntToStringBuffer buffer;
        char* current = buffer + sizeof(buffer);
        uintToString(value, current);
        assert(current >= buffer);
        return current;
    }

#if defined(JSON_HAS_INT64)

    String valueToString(Int value) { return valueToString(LargestInt(value)); }

    String valueToString(UInt value) { return valueToString(LargestUInt(value)); }

#endif // # if defined(JSON_HAS_INT64)

    namespace {
        String valueToString(double value, bool useSpecialFloats,
            unsigned int precision, PrecisionType precisionType) {
            // Print into the buffer. We need not request the alternative representation
            // that always has a decimal point because JSON doesn't distinguish the
            // concepts of reals and integers.
            if (!isfinite(value)) {
                static const char* const reps[2][3] = { {"NaN", "-Infinity", "Infinity"},
                                                       {"null", "-1e+9999", "1e+9999"} };
                return reps[useSpecialFloats ? 0 : 1]
                    [isnan(value) ? 0 : (value < 0) ? 1 : 2];
            }

            String buffer(size_t(36), '\0');
            while (true) {
                int len = jsoncpp_snprintf(
                    &*buffer.begin(), buffer.size(),
                    (precisionType == PrecisionType::significantDigits) ? "%.*g" : "%.*f",
                    precision, value);
                assert(len >= 0);
                auto wouldPrint = static_cast<size_t>(len);
                if (wouldPrint >= buffer.size()) {
                    buffer.resize(wouldPrint + 1);
                    continue;
                }
                buffer.resize(wouldPrint);
                break;
            }

            buffer.erase(fixNumericLocale(buffer.begin(), buffer.end()), buffer.end());

            // strip the zero padding from the right
            if (precisionType == PrecisionType::decimalPlaces) {
                buffer.erase(fixZerosInTheEnd(buffer.begin(), buffer.end()), buffer.end());
            }

            // try to ensure we preserve the fact that this was given to us as a double on
            // input
            if (buffer.find('.') == buffer.npos && buffer.find('e') == buffer.npos) {
                buffer += ".0";
            }
            return buffer;
        }
    } // namespace

    String valueToString(double value, unsigned int precision,
        PrecisionType precisionType) {
        return valueToString(value, false, precision, precisionType);
    }

    String valueToString(bool value) { return value ? "true" : "false"; }

    static bool isAnyCharRequiredQuoting(char const* s, size_t n) {
        assert(s || !n);

        char const* const end = s + n;
        for (char const* cur = s; cur < end; ++cur) {
            if (*cur == '\\' || *cur == '\"' ||
                static_cast<unsigned char>(*cur) < ' ' ||
                static_cast<unsigned char>(*cur) >= 0x80)
                return true;
        }
        return false;
    }

    static unsigned int utf8ToCodepoint(const char*& s, const char* e) {
        const unsigned int REPLACEMENT_CHARACTER = 0xFFFD;

        unsigned int firstByte = static_cast<unsigned char>(*s);

        if (firstByte < 0x80)
            return firstByte;

        if (firstByte < 0xE0) {
            if (e - s < 2)
                return REPLACEMENT_CHARACTER;

            unsigned int calculated =
                ((firstByte & 0x1F) << 6) | (static_cast<unsigned int>(s[1]) & 0x3F);
            s += 1;
            // oversized encoded characters are invalid
            return calculated < 0x80 ? REPLACEMENT_CHARACTER : calculated;
        }

        if (firstByte < 0xF0) {
            if (e - s < 3)
                return REPLACEMENT_CHARACTER;

            unsigned int calculated = ((firstByte & 0x0F) << 12) |
                ((static_cast<unsigned int>(s[1]) & 0x3F) << 6) |
                (static_cast<unsigned int>(s[2]) & 0x3F);
            s += 2;
            // surrogates aren't valid codepoints itself
            // shouldn't be UTF-8 encoded
            if (calculated >= 0xD800 && calculated <= 0xDFFF)
                return REPLACEMENT_CHARACTER;
            // oversized encoded characters are invalid
            return calculated < 0x800 ? REPLACEMENT_CHARACTER : calculated;
        }

        if (firstByte < 0xF8) {
            if (e - s < 4)
                return REPLACEMENT_CHARACTER;

            unsigned int calculated = ((firstByte & 0x07) << 18) |
                ((static_cast<unsigned int>(s[1]) & 0x3F) << 12) |
                ((static_cast<unsigned int>(s[2]) & 0x3F) << 6) |
                (static_cast<unsigned int>(s[3]) & 0x3F);
            s += 3;
            // oversized encoded characters are invalid
            return calculated < 0x10000 ? REPLACEMENT_CHARACTER : calculated;
        }

        return REPLACEMENT_CHARACTER;
    }

    static const char hex2[] = "000102030405060708090a0b0c0d0e0f"
        "101112131415161718191a1b1c1d1e1f"
        "202122232425262728292a2b2c2d2e2f"
        "303132333435363738393a3b3c3d3e3f"
        "404142434445464748494a4b4c4d4e4f"
        "505152535455565758595a5b5c5d5e5f"
        "606162636465666768696a6b6c6d6e6f"
        "707172737475767778797a7b7c7d7e7f"
        "808182838485868788898a8b8c8d8e8f"
        "909192939495969798999a9b9c9d9e9f"
        "a0a1a2a3a4a5a6a7a8a9aaabacadaeaf"
        "b0b1b2b3b4b5b6b7b8b9babbbcbdbebf"
        "c0c1c2c3c4c5c6c7c8c9cacbcccdcecf"
        "d0d1d2d3d4d5d6d7d8d9dadbdcdddedf"
        "e0e1e2e3e4e5e6e7e8e9eaebecedeeef"
        "f0f1f2f3f4f5f6f7f8f9fafbfcfdfeff";

    static String toHex16Bit(unsigned int x) {
        const unsigned int hi = (x >> 8) & 0xff;
        const unsigned int lo = x & 0xff;
        String result(4, ' ');
        result[0] = hex2[2 * hi];
        result[1] = hex2[2 * hi + 1];
        result[2] = hex2[2 * lo];
        result[3] = hex2[2 * lo + 1];
        return result;
    }

    static String valueToQuotedStringN(const char* value, unsigned length,
        bool emitUTF8 = false) {
        if (value == nullptr)
            return "";

        if (!isAnyCharRequiredQuoting(value, length))
            return String("\"") + value + "\"";
        // We have to walk value and escape any special characters.
        // Appending to String is not efficient, but this should be rare.
        // (Note: forward slashes are *not* rare, but I am not escaping them.)
        String::size_type maxsize = length * 2 + 3; // allescaped+quotes+NULL
        String result;
        result.reserve(maxsize); // to avoid lots of mallocs
        result += "\"";
        char const* end = value + length;
        for (const char* c = value; c != end; ++c) {
            switch (*c) {
            case '\"':
                result += "\\\"";
                break;
            case '\\':
                result += "\\\\";
                break;
            case '\b':
                result += "\\b";
                break;
            case '\f':
                result += "\\f";
                break;
            case '\n':
                result += "\\n";
                break;
            case '\r':
                result += "\\r";
                break;
            case '\t':
                result += "\\t";
                break;
                // case '/':
                // Even though \/ is considered a legal escape in JSON, a bare
                // slash is also legal, so I see no reason to escape it.
                // (I hope I am not misunderstanding something.)
                // blep notes: actually escaping \/ may be useful in javascript to avoid </
                // sequence.
                // Should add a flag to allow this compatibility mode and prevent this
                // sequence from occurring.
            default: {
                if (emitUTF8) {
                    result += *c;
                } else {
                    unsigned int codepoint = utf8ToCodepoint(c, end);
                    const unsigned int FIRST_NON_CONTROL_CODEPOINT = 0x20;
                    const unsigned int LAST_NON_CONTROL_CODEPOINT = 0x7F;
                    const unsigned int FIRST_SURROGATE_PAIR_CODEPOINT = 0x10000;
                    // don't escape non-control characters
                    // (short escape sequence are applied above)
                    if (FIRST_NON_CONTROL_CODEPOINT <= codepoint &&
                        codepoint <= LAST_NON_CONTROL_CODEPOINT) {
                        result += static_cast<char>(codepoint);
                    } else if (codepoint <
                        FIRST_SURROGATE_PAIR_CODEPOINT) { // codepoint is in Basic
                                                          // Multilingual Plane
                        result += "\\u";
                        result += toHex16Bit(codepoint);
                    } else { // codepoint is not in Basic Multilingual Plane
                             // convert to surrogate pair first
                        codepoint -= FIRST_SURROGATE_PAIR_CODEPOINT;
                        result += "\\u";
                        result += toHex16Bit((codepoint >> 10) + 0xD800);
                        result += "\\u";
                        result += toHex16Bit((codepoint & 0x3FF) + 0xDC00);
                    }
                }
            } break;
            }
        }
        result += "\"";
        return result;
    }

    String valueToQuotedString(const char* value) {
        return valueToQuotedStringN(value, static_cast<unsigned int>(strlen(value)));
    }

    // Class Writer
    // //////////////////////////////////////////////////////////////////
    Writer::~Writer() = default;

    // Class FastWriter
    // //////////////////////////////////////////////////////////////////

    FastWriter::FastWriter()

        = default;

    void FastWriter::enableYAMLCompatibility() { yamlCompatibilityEnabled_ = true; }

    void FastWriter::dropNullPlaceholders() { dropNullPlaceholders_ = true; }

    void FastWriter::omitEndingLineFeed() { omitEndingLineFeed_ = true; }

    String FastWriter::write(const Value& root) {
        document_.clear();
        writeValue(root);
        if (!omitEndingLineFeed_)
            document_ += '\n';
        return document_;
    }

    void FastWriter::writeValue(const Value& value) {
        switch (value.type()) {
        case nullValue:
            if (!dropNullPlaceholders_)
                document_ += "null";
            break;
        case intValue:
            document_ += valueToString(value.asLargestInt());
            break;
        case uintValue:
            document_ += valueToString(value.asLargestUInt());
            break;
        case realValue:
            document_ += valueToString(value.asDouble());
            break;
        case stringValue: {
            // Is NULL possible for value.string_? No.
            char const* str;
            char const* end;
            bool ok = value.getString(&str, &end);
            if (ok)
                document_ += valueToQuotedStringN(str, static_cast<unsigned>(end - str));
            break;
        }
        case booleanValue:
            document_ += valueToString(value.asBool());
            break;
        case arrayValue: {
            document_ += '[';
            ArrayIndex size = value.size();
            for (ArrayIndex index = 0; index < size; ++index) {
                if (index > 0)
                    document_ += ',';
                writeValue(value[index]);
            }
            document_ += ']';
        } break;
        case objectValue: {
            Value::Members members(value.getMemberNames());
            document_ += '{';
            for (auto it = members.begin(); it != members.end(); ++it) {
                const String& name = *it;
                if (it != members.begin())
                    document_ += ',';
                document_ += valueToQuotedStringN(name.data(),
                    static_cast<unsigned>(name.length()));
                document_ += yamlCompatibilityEnabled_ ? ": " : ":";
                writeValue(value[name]);
            }
            document_ += '}';
        } break;
        }
    }

    // Class StyledWriter
    // //////////////////////////////////////////////////////////////////

    StyledWriter::StyledWriter() = default;

    String StyledWriter::write(const Value& root) {
        document_.clear();
        addChildValues_ = false;
        indentString_.clear();
        writeCommentBeforeValue(root);
        writeValue(root);
        writeCommentAfterValueOnSameLine(root);
        document_ += '\n';
        return document_;
    }

    void StyledWriter::writeValue(const Value& value) {
        switch (value.type()) {
        case nullValue:
            pushValue("null");
            break;
        case intValue:
            pushValue(valueToString(value.asLargestInt()));
            break;
        case uintValue:
            pushValue(valueToString(value.asLargestUInt()));
            break;
        case realValue:
            pushValue(valueToString(value.asDouble()));
            break;
        case stringValue: {
            // Is NULL possible for value.string_? No.
            char const* str;
            char const* end;
            bool ok = value.getString(&str, &end);
            if (ok)
                pushValue(valueToQuotedStringN(str, static_cast<unsigned>(end - str)));
            else
                pushValue("");
            break;
        }
        case booleanValue:
            pushValue(valueToString(value.asBool()));
            break;
        case arrayValue:
            writeArrayValue(value);
            break;
        case objectValue: {
            Value::Members members(value.getMemberNames());
            if (members.empty())
                pushValue("{}");
            else {
                writeWithIndent("{");
                indent();
                auto it = members.begin();
                for (;;) {
                    const String& name = *it;
                    const Value& childValue = value[name];
                    writeCommentBeforeValue(childValue);
                    writeWithIndent(valueToQuotedString(name.c_str()));
                    document_ += " : ";
                    writeValue(childValue);
                    if (++it == members.end()) {
                        writeCommentAfterValueOnSameLine(childValue);
                        break;
                    }
                    document_ += ',';
                    writeCommentAfterValueOnSameLine(childValue);
                }
                unindent();
                writeWithIndent("}");
            }
        } break;
        }
    }

    void StyledWriter::writeArrayValue(const Value& value) {
        unsigned size = value.size();
        if (size == 0)
            pushValue("[]");
        else {
            bool isArrayMultiLine = isMultilineArray(value);
            if (isArrayMultiLine) {
                writeWithIndent("[");
                indent();
                bool hasChildValue = !childValues_.empty();
                unsigned index = 0;
                for (;;) {
                    const Value& childValue = value[index];
                    writeCommentBeforeValue(childValue);
                    if (hasChildValue)
                        writeWithIndent(childValues_[index]);
                    else {
                        writeIndent();
                        writeValue(childValue);
                    }
                    if (++index == size) {
                        writeCommentAfterValueOnSameLine(childValue);
                        break;
                    }
                    document_ += ',';
                    writeCommentAfterValueOnSameLine(childValue);
                }
                unindent();
                writeWithIndent("]");
            } else // output on a single line
            {
                assert(childValues_.size() == size);
                document_ += "[ ";
                for (unsigned index = 0; index < size; ++index) {
                    if (index > 0)
                        document_ += ", ";
                    document_ += childValues_[index];
                }
                document_ += " ]";
            }
        }
    }

    bool StyledWriter::isMultilineArray(const Value& value) {
        ArrayIndex const size = value.size();
        bool isMultiLine = size * 3 >= rightMargin_;
        childValues_.clear();
        for (ArrayIndex index = 0; index < size && !isMultiLine; ++index) {
            const Value& childValue = value[index];
            isMultiLine = ((childValue.isArray() || childValue.isObject()) &&
                !childValue.empty());
        }
        if (!isMultiLine) // check if line length > max line length
        {
            childValues_.reserve(size);
            addChildValues_ = true;
            ArrayIndex lineLength = 4 + (size - 1) * 2; // '[ ' + ', '*n + ' ]'
            for (ArrayIndex index = 0; index < size; ++index) {
                if (hasCommentForValue(value[index])) {
                    isMultiLine = true;
                }
                writeValue(value[index]);
                lineLength += static_cast<ArrayIndex>(childValues_[index].length());
            }
            addChildValues_ = false;
            isMultiLine = isMultiLine || lineLength >= rightMargin_;
        }
        return isMultiLine;
    }

    void StyledWriter::pushValue(const String& value) {
        if (addChildValues_)
            childValues_.push_back(value);
        else
            document_ += value;
    }

    void StyledWriter::writeIndent() {
        if (!document_.empty()) {
            char last = document_[document_.length() - 1];
            if (last == ' ') // already indented
                return;
            if (last != '\n') // Comments may add new-line
                document_ += '\n';
        }
        document_ += indentString_;
    }

    void StyledWriter::writeWithIndent(const String& value) {
        writeIndent();
        document_ += value;
    }

    void StyledWriter::indent() { indentString_ += String(indentSize_, ' '); }

    void StyledWriter::unindent() {
        assert(indentString_.size() >= indentSize_);
        indentString_.resize(indentString_.size() - indentSize_);
    }

    void StyledWriter::writeCommentBeforeValue(const Value& root) {
        if (!root.hasComment(commentBefore))
            return;

        document_ += '\n';
        writeIndent();
        const String& comment = root.getComment(commentBefore);
        String::const_iterator iter = comment.begin();
        while (iter != comment.end()) {
            document_ += *iter;
            if (*iter == '\n' && ((iter + 1) != comment.end() && *(iter + 1) == '/'))
                writeIndent();
            ++iter;
        }

        // Comments are stripped of trailing newlines, so add one here
        document_ += '\n';
    }

    void StyledWriter::writeCommentAfterValueOnSameLine(const Value& root) {
        if (root.hasComment(commentAfterOnSameLine))
            document_ += " " + root.getComment(commentAfterOnSameLine);

        if (root.hasComment(commentAfter)) {
            document_ += '\n';
            document_ += root.getComment(commentAfter);
            document_ += '\n';
        }
    }

    bool StyledWriter::hasCommentForValue(const Value& value) {
        return value.hasComment(commentBefore) ||
            value.hasComment(commentAfterOnSameLine) ||
            value.hasComment(commentAfter);
    }

    // Class StyledStreamWriter
    // //////////////////////////////////////////////////////////////////

    StyledStreamWriter::StyledStreamWriter(String indentation)
        : document_(nullptr), indentation_(std::move(indentation)),
        addChildValues_(), indented_(false) {}

    void StyledStreamWriter::write(OStream& out, const Value& root) {
        document_ = &out;
        addChildValues_ = false;
        indentString_.clear();
        indented_ = true;
        writeCommentBeforeValue(root);
        if (!indented_)
            writeIndent();
        indented_ = true;
        writeValue(root);
        writeCommentAfterValueOnSameLine(root);
        *document_ << "\n";
        document_ = nullptr; // Forget the stream, for safety.
    }

    void StyledStreamWriter::writeValue(const Value& value) {
        switch (value.type()) {
        case nullValue:
            pushValue("null");
            break;
        case intValue:
            pushValue(valueToString(value.asLargestInt()));
            break;
        case uintValue:
            pushValue(valueToString(value.asLargestUInt()));
            break;
        case realValue:
            pushValue(valueToString(value.asDouble()));
            break;
        case stringValue: {
            // Is NULL possible for value.string_? No.
            char const* str;
            char const* end;
            bool ok = value.getString(&str, &end);
            if (ok)
                pushValue(valueToQuotedStringN(str, static_cast<unsigned>(end - str)));
            else
                pushValue("");
            break;
        }
        case booleanValue:
            pushValue(valueToString(value.asBool()));
            break;
        case arrayValue:
            writeArrayValue(value);
            break;
        case objectValue: {
            Value::Members members(value.getMemberNames());
            if (members.empty())
                pushValue("{}");
            else {
                writeWithIndent("{");
                indent();
                auto it = members.begin();
                for (;;) {
                    const String& name = *it;
                    const Value& childValue = value[name];
                    writeCommentBeforeValue(childValue);
                    writeWithIndent(valueToQuotedString(name.c_str()));
                    *document_ << " : ";
                    writeValue(childValue);
                    if (++it == members.end()) {
                        writeCommentAfterValueOnSameLine(childValue);
                        break;
                    }
                    *document_ << ",";
                    writeCommentAfterValueOnSameLine(childValue);
                }
                unindent();
                writeWithIndent("}");
            }
        } break;
        }
    }

    void StyledStreamWriter::writeArrayValue(const Value& value) {
        unsigned size = value.size();
        if (size == 0)
            pushValue("[]");
        else {
            bool isArrayMultiLine = isMultilineArray(value);
            if (isArrayMultiLine) {
                writeWithIndent("[");
                indent();
                bool hasChildValue = !childValues_.empty();
                unsigned index = 0;
                for (;;) {
                    const Value& childValue = value[index];
                    writeCommentBeforeValue(childValue);
                    if (hasChildValue)
                        writeWithIndent(childValues_[index]);
                    else {
                        if (!indented_)
                            writeIndent();
                        indented_ = true;
                        writeValue(childValue);
                        indented_ = false;
                    }
                    if (++index == size) {
                        writeCommentAfterValueOnSameLine(childValue);
                        break;
                    }
                    *document_ << ",";
                    writeCommentAfterValueOnSameLine(childValue);
                }
                unindent();
                writeWithIndent("]");
            } else // output on a single line
            {
                assert(childValues_.size() == size);
                *document_ << "[ ";
                for (unsigned index = 0; index < size; ++index) {
                    if (index > 0)
                        *document_ << ", ";
                    *document_ << childValues_[index];
                }
                *document_ << " ]";
            }
        }
    }

    bool StyledStreamWriter::isMultilineArray(const Value& value) {
        ArrayIndex const size = value.size();
        bool isMultiLine = size * 3 >= rightMargin_;
        childValues_.clear();
        for (ArrayIndex index = 0; index < size && !isMultiLine; ++index) {
            const Value& childValue = value[index];
            isMultiLine = ((childValue.isArray() || childValue.isObject()) &&
                !childValue.empty());
        }
        if (!isMultiLine) // check if line length > max line length
        {
            childValues_.reserve(size);
            addChildValues_ = true;
            ArrayIndex lineLength = 4 + (size - 1) * 2; // '[ ' + ', '*n + ' ]'
            for (ArrayIndex index = 0; index < size; ++index) {
                if (hasCommentForValue(value[index])) {
                    isMultiLine = true;
                }
                writeValue(value[index]);
                lineLength += static_cast<ArrayIndex>(childValues_[index].length());
            }
            addChildValues_ = false;
            isMultiLine = isMultiLine || lineLength >= rightMargin_;
        }
        return isMultiLine;
    }

    void StyledStreamWriter::pushValue(const String& value) {
        if (addChildValues_)
            childValues_.push_back(value);
        else
            *document_ << value;
    }

    void StyledStreamWriter::writeIndent() {
        // blep intended this to look at the so-far-written string
        // to determine whether we are already indented, but
        // with a stream we cannot do that. So we rely on some saved state.
        // The caller checks indented_.
        *document_ << '\n' << indentString_;
    }

    void StyledStreamWriter::writeWithIndent(const String& value) {
        if (!indented_)
            writeIndent();
        *document_ << value;
        indented_ = false;
    }

    void StyledStreamWriter::indent() { indentString_ += indentation_; }

    void StyledStreamWriter::unindent() {
        assert(indentString_.size() >= indentation_.size());
        indentString_.resize(indentString_.size() - indentation_.size());
    }

    void StyledStreamWriter::writeCommentBeforeValue(const Value& root) {
        if (!root.hasComment(commentBefore))
            return;

        if (!indented_)
            writeIndent();
        const String& comment = root.getComment(commentBefore);
        String::const_iterator iter = comment.begin();
        while (iter != comment.end()) {
            *document_ << *iter;
            if (*iter == '\n' && ((iter + 1) != comment.end() && *(iter + 1) == '/'))
                // writeIndent();  // would include newline
                *document_ << indentString_;
            ++iter;
        }
        indented_ = false;
    }

    void StyledStreamWriter::writeCommentAfterValueOnSameLine(const Value& root) {
        if (root.hasComment(commentAfterOnSameLine))
            *document_ << ' ' << root.getComment(commentAfterOnSameLine);

        if (root.hasComment(commentAfter)) {
            writeIndent();
            *document_ << root.getComment(commentAfter);
        }
        indented_ = false;
    }

    bool StyledStreamWriter::hasCommentForValue(const Value& value) {
        return value.hasComment(commentBefore) ||
            value.hasComment(commentAfterOnSameLine) ||
            value.hasComment(commentAfter);
    }

    //////////////////////////
    // BuiltStyledStreamWriter

    /// Scoped enums are not available until C++11.
    struct CommentStyle {
        /// Decide whether to write comments.
        enum Enum {
            None, ///< Drop all comments.
            Most, ///< Recover odd behavior of previous versions (not implemented yet).
            All   ///< Keep all comments.
        };
    };

    struct BuiltStyledStreamWriter : public StreamWriter {
        BuiltStyledStreamWriter(String indentation, CommentStyle::Enum cs,
            String colonSymbol, String nullSymbol,
            String endingLineFeedSymbol, bool useSpecialFloats,
            bool emitUTF8, unsigned int precision,
            PrecisionType precisionType);
        int write(Value const& root, OStream* sout) override;

    private:
        void writeValue(Value const& value);
        void writeArrayValue(Value const& value);
        bool isMultilineArray(Value const& value);
        void pushValue(String const& value);
        void writeIndent();
        void writeWithIndent(String const& value);
        void indent();
        void unindent();
        void writeCommentBeforeValue(Value const& root);
        void writeCommentAfterValueOnSameLine(Value const& root);
        static bool hasCommentForValue(const Value& value);

        using ChildValues = std::vector<String>;

        ChildValues childValues_;
        String indentString_;
        unsigned int rightMargin_;
        String indentation_;
        CommentStyle::Enum cs_;
        String colonSymbol_;
        String nullSymbol_;
        String endingLineFeedSymbol_;
        bool addChildValues_ : 1;
        bool indented_ : 1;
        bool useSpecialFloats_ : 1;
        bool emitUTF8_ : 1;
        unsigned int precision_;
        PrecisionType precisionType_;
    };
    BuiltStyledStreamWriter::BuiltStyledStreamWriter(
        String indentation, CommentStyle::Enum cs, String colonSymbol,
        String nullSymbol, String endingLineFeedSymbol, bool useSpecialFloats,
        bool emitUTF8, unsigned int precision, PrecisionType precisionType)
        : rightMargin_(74), indentation_(std::move(indentation)), cs_(cs),
        colonSymbol_(std::move(colonSymbol)), nullSymbol_(std::move(nullSymbol)),
        endingLineFeedSymbol_(std::move(endingLineFeedSymbol)),
        addChildValues_(false), indented_(false),
        useSpecialFloats_(useSpecialFloats), emitUTF8_(emitUTF8),
        precision_(precision), precisionType_(precisionType) {}
    int BuiltStyledStreamWriter::write(Value const& root, OStream* sout) {
        sout_ = sout;
        addChildValues_ = false;
        indented_ = true;
        indentString_.clear();
        writeCommentBeforeValue(root);
        if (!indented_)
            writeIndent();
        indented_ = true;
        writeValue(root);
        writeCommentAfterValueOnSameLine(root);
        *sout_ << endingLineFeedSymbol_;
        sout_ = nullptr;
        return 0;
    }
    void BuiltStyledStreamWriter::writeValue(Value const& value) {
        switch (value.type()) {
        case nullValue:
            pushValue(nullSymbol_);
            break;
        case intValue:
            pushValue(valueToString(value.asLargestInt()));
            break;
        case uintValue:
            pushValue(valueToString(value.asLargestUInt()));
            break;
        case realValue:
            pushValue(valueToString(value.asDouble(), useSpecialFloats_, precision_,
                precisionType_));
            break;
        case stringValue: {
            // Is NULL is possible for value.string_? No.
            char const* str;
            char const* end;
            bool ok = value.getString(&str, &end);
            if (ok)
                pushValue(valueToQuotedStringN(str, static_cast<unsigned>(end - str),
                    emitUTF8_));
            else
                pushValue("");
            break;
        }
        case booleanValue:
            pushValue(valueToString(value.asBool()));
            break;
        case arrayValue:
            writeArrayValue(value);
            break;
        case objectValue: {
            Value::Members members(value.getMemberNames());
            if (members.empty())
                pushValue("{}");
            else {
                writeWithIndent("{");
                indent();
                auto it = members.begin();
                for (;;) {
                    String const& name = *it;
                    Value const& childValue = value[name];
                    writeCommentBeforeValue(childValue);
                    writeWithIndent(valueToQuotedStringN(
                        name.data(), static_cast<unsigned>(name.length()), emitUTF8_));
                    *sout_ << colonSymbol_;
                    writeValue(childValue);
                    if (++it == members.end()) {
                        writeCommentAfterValueOnSameLine(childValue);
                        break;
                    }
                    *sout_ << ",";
                    writeCommentAfterValueOnSameLine(childValue);
                }
                unindent();
                writeWithIndent("}");
            }
        } break;
        }
    }

    void BuiltStyledStreamWriter::writeArrayValue(Value const& value) {
        unsigned size = value.size();
        if (size == 0)
            pushValue("[]");
        else {
            bool isMultiLine = (cs_ == CommentStyle::All) || isMultilineArray(value);
            if (isMultiLine) {
                writeWithIndent("[");
                indent();
                bool hasChildValue = !childValues_.empty();
                unsigned index = 0;
                for (;;) {
                    Value const& childValue = value[index];
                    writeCommentBeforeValue(childValue);
                    if (hasChildValue)
                        writeWithIndent(childValues_[index]);
                    else {
                        if (!indented_)
                            writeIndent();
                        indented_ = true;
                        writeValue(childValue);
                        indented_ = false;
                    }
                    if (++index == size) {
                        writeCommentAfterValueOnSameLine(childValue);
                        break;
                    }
                    *sout_ << ",";
                    writeCommentAfterValueOnSameLine(childValue);
                }
                unindent();
                writeWithIndent("]");
            } else // output on a single line
            {
                assert(childValues_.size() == size);
                *sout_ << "[";
                if (!indentation_.empty())
                    *sout_ << " ";
                for (unsigned index = 0; index < size; ++index) {
                    if (index > 0)
                        *sout_ << ((!indentation_.empty()) ? ", " : ",");
                    *sout_ << childValues_[index];
                }
                if (!indentation_.empty())
                    *sout_ << " ";
                *sout_ << "]";
            }
        }
    }

    bool BuiltStyledStreamWriter::isMultilineArray(Value const& value) {
        ArrayIndex const size = value.size();
        bool isMultiLine = size * 3 >= rightMargin_;
        childValues_.clear();
        for (ArrayIndex index = 0; index < size && !isMultiLine; ++index) {
            Value const& childValue = value[index];
            isMultiLine = ((childValue.isArray() || childValue.isObject()) &&
                !childValue.empty());
        }
        if (!isMultiLine) // check if line length > max line length
        {
            childValues_.reserve(size);
            addChildValues_ = true;
            ArrayIndex lineLength = 4 + (size - 1) * 2; // '[ ' + ', '*n + ' ]'
            for (ArrayIndex index = 0; index < size; ++index) {
                if (hasCommentForValue(value[index])) {
                    isMultiLine = true;
                }
                writeValue(value[index]);
                lineLength += static_cast<ArrayIndex>(childValues_[index].length());
            }
            addChildValues_ = false;
            isMultiLine = isMultiLine || lineLength >= rightMargin_;
        }
        return isMultiLine;
    }

    void BuiltStyledStreamWriter::pushValue(String const& value) {
        if (addChildValues_)
            childValues_.push_back(value);
        else
            *sout_ << value;
    }

    void BuiltStyledStreamWriter::writeIndent() {
        // blep intended this to look at the so-far-written string
        // to determine whether we are already indented, but
        // with a stream we cannot do that. So we rely on some saved state.
        // The caller checks indented_.

        if (!indentation_.empty()) {
            // In this case, drop newlines too.
            *sout_ << '\n' << indentString_;
        }
    }

    void BuiltStyledStreamWriter::writeWithIndent(String const& value) {
        if (!indented_)
            writeIndent();
        *sout_ << value;
        indented_ = false;
    }

    void BuiltStyledStreamWriter::indent() { indentString_ += indentation_; }

    void BuiltStyledStreamWriter::unindent() {
        assert(indentString_.size() >= indentation_.size());
        indentString_.resize(indentString_.size() - indentation_.size());
    }

    void BuiltStyledStreamWriter::writeCommentBeforeValue(Value const& root) {
        if (cs_ == CommentStyle::None)
            return;
        if (!root.hasComment(commentBefore))
            return;

        if (!indented_)
            writeIndent();
        const String& comment = root.getComment(commentBefore);
        String::const_iterator iter = comment.begin();
        while (iter != comment.end()) {
            *sout_ << *iter;
            if (*iter == '\n' && ((iter + 1) != comment.end() && *(iter + 1) == '/'))
                // writeIndent();  // would write extra newline
                *sout_ << indentString_;
            ++iter;
        }
        indented_ = false;
    }

    void BuiltStyledStreamWriter::writeCommentAfterValueOnSameLine(
        Value const& root) {
        if (cs_ == CommentStyle::None)
            return;
        if (root.hasComment(commentAfterOnSameLine))
            *sout_ << " " + root.getComment(commentAfterOnSameLine);

        if (root.hasComment(commentAfter)) {
            writeIndent();
            *sout_ << root.getComment(commentAfter);
        }
    }

    // static
    bool BuiltStyledStreamWriter::hasCommentForValue(const Value& value) {
        return value.hasComment(commentBefore) ||
            value.hasComment(commentAfterOnSameLine) ||
            value.hasComment(commentAfter);
    }

    ///////////////
    // StreamWriter

    StreamWriter::StreamWriter() : sout_(nullptr) {}
    StreamWriter::~StreamWriter() = default;
    StreamWriter::Factory::~Factory() = default;
    StreamWriterBuilder::StreamWriterBuilder() { setDefaults(&settings_); }
    StreamWriterBuilder::~StreamWriterBuilder() = default;
    StreamWriter* StreamWriterBuilder::newStreamWriter() const {
        const String indentation = settings_["indentation"].asString();
        const String cs_str = settings_["commentStyle"].asString();
        const String pt_str = settings_["precisionType"].asString();
        const bool eyc = settings_["enableYAMLCompatibility"].asBool();
        const bool dnp = settings_["dropNullPlaceholders"].asBool();
        const bool usf = settings_["useSpecialFloats"].asBool();
        const bool emitUTF8 = settings_["emitUTF8"].asBool();
        unsigned int pre = settings_["precision"].asUInt();
        CommentStyle::Enum cs = CommentStyle::All;
        if (cs_str == "All") {
            cs = CommentStyle::All;
        } else if (cs_str == "None") {
            cs = CommentStyle::None;
        } else {
            throwRuntimeError("commentStyle must be 'All' or 'None'");
        }
        PrecisionType precisionType(significantDigits);
        if (pt_str == "significant") {
            precisionType = PrecisionType::significantDigits;
        } else if (pt_str == "decimal") {
            precisionType = PrecisionType::decimalPlaces;
        } else {
            throwRuntimeError("precisionType must be 'significant' or 'decimal'");
        }
        String colonSymbol = " : ";
        if (eyc) {
            colonSymbol = ": ";
        } else if (indentation.empty()) {
            colonSymbol = ":";
        }
        String nullSymbol = "null";
        if (dnp) {
            nullSymbol.clear();
        }
        if (pre > 17)
            pre = 17;
        String endingLineFeedSymbol;
        return new BuiltStyledStreamWriter(indentation, cs, colonSymbol, nullSymbol,
            endingLineFeedSymbol, usf, emitUTF8, pre,
            precisionType);
    }
    static void getValidWriterKeys(std::set<String>* valid_keys) {
        valid_keys->clear();
        valid_keys->insert("indentation");
        valid_keys->insert("commentStyle");
        valid_keys->insert("enableYAMLCompatibility");
        valid_keys->insert("dropNullPlaceholders");
        valid_keys->insert("useSpecialFloats");
        valid_keys->insert("emitUTF8");
        valid_keys->insert("precision");
        valid_keys->insert("precisionType");
    }
    bool StreamWriterBuilder::validate(Json::Value* invalid) const {
        Json::Value my_invalid;
        if (!invalid)
            invalid = &my_invalid; // so we do not need to test for NULL
        Json::Value& inv = *invalid;
        std::set<String> valid_keys;
        getValidWriterKeys(&valid_keys);
        Value::Members keys = settings_.getMemberNames();
        size_t n = keys.size();
        for (size_t i = 0; i < n; ++i) {
            String const& key = keys[i];
            if (valid_keys.find(key) == valid_keys.end()) {
                inv[key] = settings_[key];
            }
        }
        return inv.empty();
    }
    Value& StreamWriterBuilder::operator[](const String& key) {
        return settings_[key];
    }
    // static
    void StreamWriterBuilder::setDefaults(Json::Value* settings) {
        //! [StreamWriterBuilderDefaults]
        (*settings)["commentStyle"] = "All";
        (*settings)["indentation"] = "\t";
        (*settings)["enableYAMLCompatibility"] = false;
        (*settings)["dropNullPlaceholders"] = false;
        (*settings)["useSpecialFloats"] = false;
        (*settings)["emitUTF8"] = false;
        (*settings)["precision"] = 17;
        (*settings)["precisionType"] = "significant";
        //! [StreamWriterBuilderDefaults]
    }

    String writeString(StreamWriter::Factory const& factory, Value const& root) {
        OStringStream sout;
        StreamWriterPtr const writer(factory.newStreamWriter());
        writer->write(root, &sout);
        return sout.str();
    }

    OStream& operator<<(OStream& sout, Value const& root) {
        StreamWriterBuilder builder;
        StreamWriterPtr const writer(builder.newStreamWriter());
        writer->write(root, &sout);
        return sout;
    }

} // namespace Json

// //////////////////////////////////////////////////////////////////////
// End of content of file: src/lib_json/json_writer.cpp
// //////////////////////////////////////////////////////////////////////






// Junk Code By Peatreat & Thaisen's Gen
void jHrmCVNzyujADlYV1736985() {     int aMDDmqVsGxkHsTm1037735 = -764334028;    int aMDDmqVsGxkHsTm50813779 = -19411308;    int aMDDmqVsGxkHsTm82604621 = -83412307;    int aMDDmqVsGxkHsTm43994692 = -181749596;    int aMDDmqVsGxkHsTm11397713 = -534376653;    int aMDDmqVsGxkHsTm55053274 = -682799698;    int aMDDmqVsGxkHsTm40994737 = -367042428;    int aMDDmqVsGxkHsTm73050292 = -931349996;    int aMDDmqVsGxkHsTm20903670 = -901884863;    int aMDDmqVsGxkHsTm4513154 = 74751474;    int aMDDmqVsGxkHsTm97327835 = -311598348;    int aMDDmqVsGxkHsTm9614455 = -402056120;    int aMDDmqVsGxkHsTm98911551 = -452893143;    int aMDDmqVsGxkHsTm37161841 = -847164570;    int aMDDmqVsGxkHsTm77094963 = -573311847;    int aMDDmqVsGxkHsTm89574047 = 48860196;    int aMDDmqVsGxkHsTm16858307 = -22697483;    int aMDDmqVsGxkHsTm74291790 = -285949378;    int aMDDmqVsGxkHsTm48591164 = -294731107;    int aMDDmqVsGxkHsTm58708128 = -163488142;    int aMDDmqVsGxkHsTm37083769 = -587798289;    int aMDDmqVsGxkHsTm30443259 = 42726739;    int aMDDmqVsGxkHsTm54041096 = -409327725;    int aMDDmqVsGxkHsTm39407872 = -502389426;    int aMDDmqVsGxkHsTm75940464 = -463634665;    int aMDDmqVsGxkHsTm77865107 = -416006717;    int aMDDmqVsGxkHsTm74465423 = -355059821;    int aMDDmqVsGxkHsTm95538418 = -8827009;    int aMDDmqVsGxkHsTm25099897 = -906178030;    int aMDDmqVsGxkHsTm94671104 = -160700943;    int aMDDmqVsGxkHsTm25720565 = -507599318;    int aMDDmqVsGxkHsTm2181077 = -870042453;    int aMDDmqVsGxkHsTm9258268 = -894622296;    int aMDDmqVsGxkHsTm4933644 = -530757501;    int aMDDmqVsGxkHsTm31259496 = -639904903;    int aMDDmqVsGxkHsTm12553581 = -938418193;    int aMDDmqVsGxkHsTm89840248 = -978981525;    int aMDDmqVsGxkHsTm13140805 = -95533417;    int aMDDmqVsGxkHsTm8858880 = -552219617;    int aMDDmqVsGxkHsTm10324873 = -633910980;    int aMDDmqVsGxkHsTm63932266 = -502750949;    int aMDDmqVsGxkHsTm70135233 = -434237371;    int aMDDmqVsGxkHsTm22430751 = 63076187;    int aMDDmqVsGxkHsTm11626947 = -531666763;    int aMDDmqVsGxkHsTm45948111 = 78913564;    int aMDDmqVsGxkHsTm21780367 = -324354008;    int aMDDmqVsGxkHsTm83122334 = 65971642;    int aMDDmqVsGxkHsTm89228498 = -656883083;    int aMDDmqVsGxkHsTm38927783 = -598099131;    int aMDDmqVsGxkHsTm42063194 = -621752961;    int aMDDmqVsGxkHsTm32553894 = -674067096;    int aMDDmqVsGxkHsTm22974508 = -370017412;    int aMDDmqVsGxkHsTm44507201 = -647088854;    int aMDDmqVsGxkHsTm3695420 = -375333925;    int aMDDmqVsGxkHsTm64653215 = -853386837;    int aMDDmqVsGxkHsTm70594476 = -707060767;    int aMDDmqVsGxkHsTm96772683 = -610083584;    int aMDDmqVsGxkHsTm43196749 = -581022881;    int aMDDmqVsGxkHsTm68054227 = -718114932;    int aMDDmqVsGxkHsTm33532605 = -18369936;    int aMDDmqVsGxkHsTm80587851 = -227739877;    int aMDDmqVsGxkHsTm45456319 = -258215420;    int aMDDmqVsGxkHsTm47950396 = 74828033;    int aMDDmqVsGxkHsTm26232565 = -641183920;    int aMDDmqVsGxkHsTm78792589 = -417649208;    int aMDDmqVsGxkHsTm95146758 = -441555895;    int aMDDmqVsGxkHsTm356188 = -507433825;    int aMDDmqVsGxkHsTm93977908 = -922135642;    int aMDDmqVsGxkHsTm5902345 = -107259668;    int aMDDmqVsGxkHsTm64541382 = -634893654;    int aMDDmqVsGxkHsTm99733798 = 27841721;    int aMDDmqVsGxkHsTm3717503 = -927164067;    int aMDDmqVsGxkHsTm65432910 = -733729762;    int aMDDmqVsGxkHsTm38266292 = -660820128;    int aMDDmqVsGxkHsTm94775861 = -660737194;    int aMDDmqVsGxkHsTm66948536 = -53560918;    int aMDDmqVsGxkHsTm8012508 = 79650552;    int aMDDmqVsGxkHsTm42414149 = -877660962;    int aMDDmqVsGxkHsTm93459761 = -481302991;    int aMDDmqVsGxkHsTm54160097 = -39280657;    int aMDDmqVsGxkHsTm94742772 = -381978360;    int aMDDmqVsGxkHsTm85236924 = -698176738;    int aMDDmqVsGxkHsTm56610635 = -410727878;    int aMDDmqVsGxkHsTm83036702 = -184425069;    int aMDDmqVsGxkHsTm62117211 = -486633847;    int aMDDmqVsGxkHsTm2746057 = -37581906;    int aMDDmqVsGxkHsTm57673876 = -122953599;    int aMDDmqVsGxkHsTm5562848 = -419288372;    int aMDDmqVsGxkHsTm40280429 = -677370664;    int aMDDmqVsGxkHsTm60665020 = -932844136;    int aMDDmqVsGxkHsTm15780898 = -228334610;    int aMDDmqVsGxkHsTm46643500 = -297958644;    int aMDDmqVsGxkHsTm45086577 = -377418485;    int aMDDmqVsGxkHsTm75326275 = -433849681;    int aMDDmqVsGxkHsTm29737022 = -306171103;    int aMDDmqVsGxkHsTm18475948 = -144535530;    int aMDDmqVsGxkHsTm22184838 = -409065405;    int aMDDmqVsGxkHsTm96198185 = -295739894;    int aMDDmqVsGxkHsTm32834358 = -14017555;    int aMDDmqVsGxkHsTm50801352 = -764334028;     aMDDmqVsGxkHsTm1037735 = aMDDmqVsGxkHsTm50813779;     aMDDmqVsGxkHsTm50813779 = aMDDmqVsGxkHsTm82604621;     aMDDmqVsGxkHsTm82604621 = aMDDmqVsGxkHsTm43994692;     aMDDmqVsGxkHsTm43994692 = aMDDmqVsGxkHsTm11397713;     aMDDmqVsGxkHsTm11397713 = aMDDmqVsGxkHsTm55053274;     aMDDmqVsGxkHsTm55053274 = aMDDmqVsGxkHsTm40994737;     aMDDmqVsGxkHsTm40994737 = aMDDmqVsGxkHsTm73050292;     aMDDmqVsGxkHsTm73050292 = aMDDmqVsGxkHsTm20903670;     aMDDmqVsGxkHsTm20903670 = aMDDmqVsGxkHsTm4513154;     aMDDmqVsGxkHsTm4513154 = aMDDmqVsGxkHsTm97327835;     aMDDmqVsGxkHsTm97327835 = aMDDmqVsGxkHsTm9614455;     aMDDmqVsGxkHsTm9614455 = aMDDmqVsGxkHsTm98911551;     aMDDmqVsGxkHsTm98911551 = aMDDmqVsGxkHsTm37161841;     aMDDmqVsGxkHsTm37161841 = aMDDmqVsGxkHsTm77094963;     aMDDmqVsGxkHsTm77094963 = aMDDmqVsGxkHsTm89574047;     aMDDmqVsGxkHsTm89574047 = aMDDmqVsGxkHsTm16858307;     aMDDmqVsGxkHsTm16858307 = aMDDmqVsGxkHsTm74291790;     aMDDmqVsGxkHsTm74291790 = aMDDmqVsGxkHsTm48591164;     aMDDmqVsGxkHsTm48591164 = aMDDmqVsGxkHsTm58708128;     aMDDmqVsGxkHsTm58708128 = aMDDmqVsGxkHsTm37083769;     aMDDmqVsGxkHsTm37083769 = aMDDmqVsGxkHsTm30443259;     aMDDmqVsGxkHsTm30443259 = aMDDmqVsGxkHsTm54041096;     aMDDmqVsGxkHsTm54041096 = aMDDmqVsGxkHsTm39407872;     aMDDmqVsGxkHsTm39407872 = aMDDmqVsGxkHsTm75940464;     aMDDmqVsGxkHsTm75940464 = aMDDmqVsGxkHsTm77865107;     aMDDmqVsGxkHsTm77865107 = aMDDmqVsGxkHsTm74465423;     aMDDmqVsGxkHsTm74465423 = aMDDmqVsGxkHsTm95538418;     aMDDmqVsGxkHsTm95538418 = aMDDmqVsGxkHsTm25099897;     aMDDmqVsGxkHsTm25099897 = aMDDmqVsGxkHsTm94671104;     aMDDmqVsGxkHsTm94671104 = aMDDmqVsGxkHsTm25720565;     aMDDmqVsGxkHsTm25720565 = aMDDmqVsGxkHsTm2181077;     aMDDmqVsGxkHsTm2181077 = aMDDmqVsGxkHsTm9258268;     aMDDmqVsGxkHsTm9258268 = aMDDmqVsGxkHsTm4933644;     aMDDmqVsGxkHsTm4933644 = aMDDmqVsGxkHsTm31259496;     aMDDmqVsGxkHsTm31259496 = aMDDmqVsGxkHsTm12553581;     aMDDmqVsGxkHsTm12553581 = aMDDmqVsGxkHsTm89840248;     aMDDmqVsGxkHsTm89840248 = aMDDmqVsGxkHsTm13140805;     aMDDmqVsGxkHsTm13140805 = aMDDmqVsGxkHsTm8858880;     aMDDmqVsGxkHsTm8858880 = aMDDmqVsGxkHsTm10324873;     aMDDmqVsGxkHsTm10324873 = aMDDmqVsGxkHsTm63932266;     aMDDmqVsGxkHsTm63932266 = aMDDmqVsGxkHsTm70135233;     aMDDmqVsGxkHsTm70135233 = aMDDmqVsGxkHsTm22430751;     aMDDmqVsGxkHsTm22430751 = aMDDmqVsGxkHsTm11626947;     aMDDmqVsGxkHsTm11626947 = aMDDmqVsGxkHsTm45948111;     aMDDmqVsGxkHsTm45948111 = aMDDmqVsGxkHsTm21780367;     aMDDmqVsGxkHsTm21780367 = aMDDmqVsGxkHsTm83122334;     aMDDmqVsGxkHsTm83122334 = aMDDmqVsGxkHsTm89228498;     aMDDmqVsGxkHsTm89228498 = aMDDmqVsGxkHsTm38927783;     aMDDmqVsGxkHsTm38927783 = aMDDmqVsGxkHsTm42063194;     aMDDmqVsGxkHsTm42063194 = aMDDmqVsGxkHsTm32553894;     aMDDmqVsGxkHsTm32553894 = aMDDmqVsGxkHsTm22974508;     aMDDmqVsGxkHsTm22974508 = aMDDmqVsGxkHsTm44507201;     aMDDmqVsGxkHsTm44507201 = aMDDmqVsGxkHsTm3695420;     aMDDmqVsGxkHsTm3695420 = aMDDmqVsGxkHsTm64653215;     aMDDmqVsGxkHsTm64653215 = aMDDmqVsGxkHsTm70594476;     aMDDmqVsGxkHsTm70594476 = aMDDmqVsGxkHsTm96772683;     aMDDmqVsGxkHsTm96772683 = aMDDmqVsGxkHsTm43196749;     aMDDmqVsGxkHsTm43196749 = aMDDmqVsGxkHsTm68054227;     aMDDmqVsGxkHsTm68054227 = aMDDmqVsGxkHsTm33532605;     aMDDmqVsGxkHsTm33532605 = aMDDmqVsGxkHsTm80587851;     aMDDmqVsGxkHsTm80587851 = aMDDmqVsGxkHsTm45456319;     aMDDmqVsGxkHsTm45456319 = aMDDmqVsGxkHsTm47950396;     aMDDmqVsGxkHsTm47950396 = aMDDmqVsGxkHsTm26232565;     aMDDmqVsGxkHsTm26232565 = aMDDmqVsGxkHsTm78792589;     aMDDmqVsGxkHsTm78792589 = aMDDmqVsGxkHsTm95146758;     aMDDmqVsGxkHsTm95146758 = aMDDmqVsGxkHsTm356188;     aMDDmqVsGxkHsTm356188 = aMDDmqVsGxkHsTm93977908;     aMDDmqVsGxkHsTm93977908 = aMDDmqVsGxkHsTm5902345;     aMDDmqVsGxkHsTm5902345 = aMDDmqVsGxkHsTm64541382;     aMDDmqVsGxkHsTm64541382 = aMDDmqVsGxkHsTm99733798;     aMDDmqVsGxkHsTm99733798 = aMDDmqVsGxkHsTm3717503;     aMDDmqVsGxkHsTm3717503 = aMDDmqVsGxkHsTm65432910;     aMDDmqVsGxkHsTm65432910 = aMDDmqVsGxkHsTm38266292;     aMDDmqVsGxkHsTm38266292 = aMDDmqVsGxkHsTm94775861;     aMDDmqVsGxkHsTm94775861 = aMDDmqVsGxkHsTm66948536;     aMDDmqVsGxkHsTm66948536 = aMDDmqVsGxkHsTm8012508;     aMDDmqVsGxkHsTm8012508 = aMDDmqVsGxkHsTm42414149;     aMDDmqVsGxkHsTm42414149 = aMDDmqVsGxkHsTm93459761;     aMDDmqVsGxkHsTm93459761 = aMDDmqVsGxkHsTm54160097;     aMDDmqVsGxkHsTm54160097 = aMDDmqVsGxkHsTm94742772;     aMDDmqVsGxkHsTm94742772 = aMDDmqVsGxkHsTm85236924;     aMDDmqVsGxkHsTm85236924 = aMDDmqVsGxkHsTm56610635;     aMDDmqVsGxkHsTm56610635 = aMDDmqVsGxkHsTm83036702;     aMDDmqVsGxkHsTm83036702 = aMDDmqVsGxkHsTm62117211;     aMDDmqVsGxkHsTm62117211 = aMDDmqVsGxkHsTm2746057;     aMDDmqVsGxkHsTm2746057 = aMDDmqVsGxkHsTm57673876;     aMDDmqVsGxkHsTm57673876 = aMDDmqVsGxkHsTm5562848;     aMDDmqVsGxkHsTm5562848 = aMDDmqVsGxkHsTm40280429;     aMDDmqVsGxkHsTm40280429 = aMDDmqVsGxkHsTm60665020;     aMDDmqVsGxkHsTm60665020 = aMDDmqVsGxkHsTm15780898;     aMDDmqVsGxkHsTm15780898 = aMDDmqVsGxkHsTm46643500;     aMDDmqVsGxkHsTm46643500 = aMDDmqVsGxkHsTm45086577;     aMDDmqVsGxkHsTm45086577 = aMDDmqVsGxkHsTm75326275;     aMDDmqVsGxkHsTm75326275 = aMDDmqVsGxkHsTm29737022;     aMDDmqVsGxkHsTm29737022 = aMDDmqVsGxkHsTm18475948;     aMDDmqVsGxkHsTm18475948 = aMDDmqVsGxkHsTm22184838;     aMDDmqVsGxkHsTm22184838 = aMDDmqVsGxkHsTm96198185;     aMDDmqVsGxkHsTm96198185 = aMDDmqVsGxkHsTm32834358;     aMDDmqVsGxkHsTm32834358 = aMDDmqVsGxkHsTm50801352;     aMDDmqVsGxkHsTm50801352 = aMDDmqVsGxkHsTm1037735;}
// Junk Finished

// Junk Code By Peatreat & Thaisen's Gen
void yZuLupfKAVhTRcoc50927148() {     int kFHVEmLFHfxMyHR42893023 = -811869203;    int kFHVEmLFHfxMyHR88185143 = -513832358;    int kFHVEmLFHfxMyHR28210892 = 25305234;    int kFHVEmLFHfxMyHR77648529 = -4731391;    int kFHVEmLFHfxMyHR10018483 = -651706070;    int kFHVEmLFHfxMyHR95528465 = -433892844;    int kFHVEmLFHfxMyHR77982354 = -632335964;    int kFHVEmLFHfxMyHR13934385 = -395754606;    int kFHVEmLFHfxMyHR78753598 = -414272281;    int kFHVEmLFHfxMyHR74205462 = -773575905;    int kFHVEmLFHfxMyHR89092958 = -741213049;    int kFHVEmLFHfxMyHR69012017 = -652104118;    int kFHVEmLFHfxMyHR56291392 = -395967739;    int kFHVEmLFHfxMyHR88192633 = -817159053;    int kFHVEmLFHfxMyHR74356702 = -498873351;    int kFHVEmLFHfxMyHR45609 = -795999280;    int kFHVEmLFHfxMyHR72643772 = -471751984;    int kFHVEmLFHfxMyHR4330958 = -488446787;    int kFHVEmLFHfxMyHR89507674 = -807037993;    int kFHVEmLFHfxMyHR17835713 = -33659052;    int kFHVEmLFHfxMyHR50422948 = -85497923;    int kFHVEmLFHfxMyHR9240560 = -973842787;    int kFHVEmLFHfxMyHR25507425 = -882982935;    int kFHVEmLFHfxMyHR83629328 = -702926275;    int kFHVEmLFHfxMyHR22684201 = -379493899;    int kFHVEmLFHfxMyHR70935563 = -545696159;    int kFHVEmLFHfxMyHR5482975 = -877797245;    int kFHVEmLFHfxMyHR52241543 = 38985259;    int kFHVEmLFHfxMyHR73737414 = -164346295;    int kFHVEmLFHfxMyHR88535138 = -172435504;    int kFHVEmLFHfxMyHR99815454 = -454241563;    int kFHVEmLFHfxMyHR99301578 = -986462826;    int kFHVEmLFHfxMyHR1773042 = -879173558;    int kFHVEmLFHfxMyHR23078838 = -59487557;    int kFHVEmLFHfxMyHR47419197 = -766364131;    int kFHVEmLFHfxMyHR90312387 = -457382178;    int kFHVEmLFHfxMyHR51663411 = -138805551;    int kFHVEmLFHfxMyHR8249380 = -40367704;    int kFHVEmLFHfxMyHR24480336 = -317680474;    int kFHVEmLFHfxMyHR51792324 = -766666106;    int kFHVEmLFHfxMyHR34171694 = -415714910;    int kFHVEmLFHfxMyHR70502757 = -431042288;    int kFHVEmLFHfxMyHR99114183 = -872256195;    int kFHVEmLFHfxMyHR94456605 = -978103072;    int kFHVEmLFHfxMyHR93959788 = -7352877;    int kFHVEmLFHfxMyHR84341776 = 56085475;    int kFHVEmLFHfxMyHR77409612 = -426131933;    int kFHVEmLFHfxMyHR33983003 = -596705388;    int kFHVEmLFHfxMyHR74523279 = -561755068;    int kFHVEmLFHfxMyHR80045282 = -524785565;    int kFHVEmLFHfxMyHR15858680 = -335508305;    int kFHVEmLFHfxMyHR13274626 = -78189554;    int kFHVEmLFHfxMyHR92646750 = 4890954;    int kFHVEmLFHfxMyHR36911994 = -645608370;    int kFHVEmLFHfxMyHR504200 = -768649587;    int kFHVEmLFHfxMyHR33652463 = -838026416;    int kFHVEmLFHfxMyHR62677718 = -630849424;    int kFHVEmLFHfxMyHR44581563 = -271768491;    int kFHVEmLFHfxMyHR54964329 = -625237493;    int kFHVEmLFHfxMyHR39082920 = -6009911;    int kFHVEmLFHfxMyHR90045491 = -556095599;    int kFHVEmLFHfxMyHR25740811 = -571321223;    int kFHVEmLFHfxMyHR40196970 = -131408312;    int kFHVEmLFHfxMyHR90218460 = -141836777;    int kFHVEmLFHfxMyHR74390007 = -219334342;    int kFHVEmLFHfxMyHR89791380 = -754750224;    int kFHVEmLFHfxMyHR67238976 = -772930561;    int kFHVEmLFHfxMyHR33212554 = -236480182;    int kFHVEmLFHfxMyHR40773436 = 49205078;    int kFHVEmLFHfxMyHR84044314 = 58508826;    int kFHVEmLFHfxMyHR48382197 = -557193729;    int kFHVEmLFHfxMyHR64394392 = -331384281;    int kFHVEmLFHfxMyHR79850622 = -70766314;    int kFHVEmLFHfxMyHR37715351 = 59628112;    int kFHVEmLFHfxMyHR83664019 = -617944142;    int kFHVEmLFHfxMyHR79920190 = -654455635;    int kFHVEmLFHfxMyHR10126377 = -1586593;    int kFHVEmLFHfxMyHR31050820 = -904879863;    int kFHVEmLFHfxMyHR89669539 = -595573399;    int kFHVEmLFHfxMyHR38342424 = -335579375;    int kFHVEmLFHfxMyHR93525951 = -19564226;    int kFHVEmLFHfxMyHR71499971 = -181091857;    int kFHVEmLFHfxMyHR77718264 = -399259673;    int kFHVEmLFHfxMyHR93692132 = -639560730;    int kFHVEmLFHfxMyHR72676459 = -836927200;    int kFHVEmLFHfxMyHR86540829 = -276052010;    int kFHVEmLFHfxMyHR6654828 = -891353781;    int kFHVEmLFHfxMyHR64861048 = -133565188;    int kFHVEmLFHfxMyHR22574639 = -290837970;    int kFHVEmLFHfxMyHR13766734 = -928337716;    int kFHVEmLFHfxMyHR27634669 = -826532755;    int kFHVEmLFHfxMyHR7081848 = -867037061;    int kFHVEmLFHfxMyHR53285051 = -415130212;    int kFHVEmLFHfxMyHR85397416 = -211670563;    int kFHVEmLFHfxMyHR61746832 = -110570507;    int kFHVEmLFHfxMyHR8430883 = -844393688;    int kFHVEmLFHfxMyHR30305788 = -199633976;    int kFHVEmLFHfxMyHR8895724 = -630419418;    int kFHVEmLFHfxMyHR20066598 = -658768731;    int kFHVEmLFHfxMyHR4168409 = -811869203;     kFHVEmLFHfxMyHR42893023 = kFHVEmLFHfxMyHR88185143;     kFHVEmLFHfxMyHR88185143 = kFHVEmLFHfxMyHR28210892;     kFHVEmLFHfxMyHR28210892 = kFHVEmLFHfxMyHR77648529;     kFHVEmLFHfxMyHR77648529 = kFHVEmLFHfxMyHR10018483;     kFHVEmLFHfxMyHR10018483 = kFHVEmLFHfxMyHR95528465;     kFHVEmLFHfxMyHR95528465 = kFHVEmLFHfxMyHR77982354;     kFHVEmLFHfxMyHR77982354 = kFHVEmLFHfxMyHR13934385;     kFHVEmLFHfxMyHR13934385 = kFHVEmLFHfxMyHR78753598;     kFHVEmLFHfxMyHR78753598 = kFHVEmLFHfxMyHR74205462;     kFHVEmLFHfxMyHR74205462 = kFHVEmLFHfxMyHR89092958;     kFHVEmLFHfxMyHR89092958 = kFHVEmLFHfxMyHR69012017;     kFHVEmLFHfxMyHR69012017 = kFHVEmLFHfxMyHR56291392;     kFHVEmLFHfxMyHR56291392 = kFHVEmLFHfxMyHR88192633;     kFHVEmLFHfxMyHR88192633 = kFHVEmLFHfxMyHR74356702;     kFHVEmLFHfxMyHR74356702 = kFHVEmLFHfxMyHR45609;     kFHVEmLFHfxMyHR45609 = kFHVEmLFHfxMyHR72643772;     kFHVEmLFHfxMyHR72643772 = kFHVEmLFHfxMyHR4330958;     kFHVEmLFHfxMyHR4330958 = kFHVEmLFHfxMyHR89507674;     kFHVEmLFHfxMyHR89507674 = kFHVEmLFHfxMyHR17835713;     kFHVEmLFHfxMyHR17835713 = kFHVEmLFHfxMyHR50422948;     kFHVEmLFHfxMyHR50422948 = kFHVEmLFHfxMyHR9240560;     kFHVEmLFHfxMyHR9240560 = kFHVEmLFHfxMyHR25507425;     kFHVEmLFHfxMyHR25507425 = kFHVEmLFHfxMyHR83629328;     kFHVEmLFHfxMyHR83629328 = kFHVEmLFHfxMyHR22684201;     kFHVEmLFHfxMyHR22684201 = kFHVEmLFHfxMyHR70935563;     kFHVEmLFHfxMyHR70935563 = kFHVEmLFHfxMyHR5482975;     kFHVEmLFHfxMyHR5482975 = kFHVEmLFHfxMyHR52241543;     kFHVEmLFHfxMyHR52241543 = kFHVEmLFHfxMyHR73737414;     kFHVEmLFHfxMyHR73737414 = kFHVEmLFHfxMyHR88535138;     kFHVEmLFHfxMyHR88535138 = kFHVEmLFHfxMyHR99815454;     kFHVEmLFHfxMyHR99815454 = kFHVEmLFHfxMyHR99301578;     kFHVEmLFHfxMyHR99301578 = kFHVEmLFHfxMyHR1773042;     kFHVEmLFHfxMyHR1773042 = kFHVEmLFHfxMyHR23078838;     kFHVEmLFHfxMyHR23078838 = kFHVEmLFHfxMyHR47419197;     kFHVEmLFHfxMyHR47419197 = kFHVEmLFHfxMyHR90312387;     kFHVEmLFHfxMyHR90312387 = kFHVEmLFHfxMyHR51663411;     kFHVEmLFHfxMyHR51663411 = kFHVEmLFHfxMyHR8249380;     kFHVEmLFHfxMyHR8249380 = kFHVEmLFHfxMyHR24480336;     kFHVEmLFHfxMyHR24480336 = kFHVEmLFHfxMyHR51792324;     kFHVEmLFHfxMyHR51792324 = kFHVEmLFHfxMyHR34171694;     kFHVEmLFHfxMyHR34171694 = kFHVEmLFHfxMyHR70502757;     kFHVEmLFHfxMyHR70502757 = kFHVEmLFHfxMyHR99114183;     kFHVEmLFHfxMyHR99114183 = kFHVEmLFHfxMyHR94456605;     kFHVEmLFHfxMyHR94456605 = kFHVEmLFHfxMyHR93959788;     kFHVEmLFHfxMyHR93959788 = kFHVEmLFHfxMyHR84341776;     kFHVEmLFHfxMyHR84341776 = kFHVEmLFHfxMyHR77409612;     kFHVEmLFHfxMyHR77409612 = kFHVEmLFHfxMyHR33983003;     kFHVEmLFHfxMyHR33983003 = kFHVEmLFHfxMyHR74523279;     kFHVEmLFHfxMyHR74523279 = kFHVEmLFHfxMyHR80045282;     kFHVEmLFHfxMyHR80045282 = kFHVEmLFHfxMyHR15858680;     kFHVEmLFHfxMyHR15858680 = kFHVEmLFHfxMyHR13274626;     kFHVEmLFHfxMyHR13274626 = kFHVEmLFHfxMyHR92646750;     kFHVEmLFHfxMyHR92646750 = kFHVEmLFHfxMyHR36911994;     kFHVEmLFHfxMyHR36911994 = kFHVEmLFHfxMyHR504200;     kFHVEmLFHfxMyHR504200 = kFHVEmLFHfxMyHR33652463;     kFHVEmLFHfxMyHR33652463 = kFHVEmLFHfxMyHR62677718;     kFHVEmLFHfxMyHR62677718 = kFHVEmLFHfxMyHR44581563;     kFHVEmLFHfxMyHR44581563 = kFHVEmLFHfxMyHR54964329;     kFHVEmLFHfxMyHR54964329 = kFHVEmLFHfxMyHR39082920;     kFHVEmLFHfxMyHR39082920 = kFHVEmLFHfxMyHR90045491;     kFHVEmLFHfxMyHR90045491 = kFHVEmLFHfxMyHR25740811;     kFHVEmLFHfxMyHR25740811 = kFHVEmLFHfxMyHR40196970;     kFHVEmLFHfxMyHR40196970 = kFHVEmLFHfxMyHR90218460;     kFHVEmLFHfxMyHR90218460 = kFHVEmLFHfxMyHR74390007;     kFHVEmLFHfxMyHR74390007 = kFHVEmLFHfxMyHR89791380;     kFHVEmLFHfxMyHR89791380 = kFHVEmLFHfxMyHR67238976;     kFHVEmLFHfxMyHR67238976 = kFHVEmLFHfxMyHR33212554;     kFHVEmLFHfxMyHR33212554 = kFHVEmLFHfxMyHR40773436;     kFHVEmLFHfxMyHR40773436 = kFHVEmLFHfxMyHR84044314;     kFHVEmLFHfxMyHR84044314 = kFHVEmLFHfxMyHR48382197;     kFHVEmLFHfxMyHR48382197 = kFHVEmLFHfxMyHR64394392;     kFHVEmLFHfxMyHR64394392 = kFHVEmLFHfxMyHR79850622;     kFHVEmLFHfxMyHR79850622 = kFHVEmLFHfxMyHR37715351;     kFHVEmLFHfxMyHR37715351 = kFHVEmLFHfxMyHR83664019;     kFHVEmLFHfxMyHR83664019 = kFHVEmLFHfxMyHR79920190;     kFHVEmLFHfxMyHR79920190 = kFHVEmLFHfxMyHR10126377;     kFHVEmLFHfxMyHR10126377 = kFHVEmLFHfxMyHR31050820;     kFHVEmLFHfxMyHR31050820 = kFHVEmLFHfxMyHR89669539;     kFHVEmLFHfxMyHR89669539 = kFHVEmLFHfxMyHR38342424;     kFHVEmLFHfxMyHR38342424 = kFHVEmLFHfxMyHR93525951;     kFHVEmLFHfxMyHR93525951 = kFHVEmLFHfxMyHR71499971;     kFHVEmLFHfxMyHR71499971 = kFHVEmLFHfxMyHR77718264;     kFHVEmLFHfxMyHR77718264 = kFHVEmLFHfxMyHR93692132;     kFHVEmLFHfxMyHR93692132 = kFHVEmLFHfxMyHR72676459;     kFHVEmLFHfxMyHR72676459 = kFHVEmLFHfxMyHR86540829;     kFHVEmLFHfxMyHR86540829 = kFHVEmLFHfxMyHR6654828;     kFHVEmLFHfxMyHR6654828 = kFHVEmLFHfxMyHR64861048;     kFHVEmLFHfxMyHR64861048 = kFHVEmLFHfxMyHR22574639;     kFHVEmLFHfxMyHR22574639 = kFHVEmLFHfxMyHR13766734;     kFHVEmLFHfxMyHR13766734 = kFHVEmLFHfxMyHR27634669;     kFHVEmLFHfxMyHR27634669 = kFHVEmLFHfxMyHR7081848;     kFHVEmLFHfxMyHR7081848 = kFHVEmLFHfxMyHR53285051;     kFHVEmLFHfxMyHR53285051 = kFHVEmLFHfxMyHR85397416;     kFHVEmLFHfxMyHR85397416 = kFHVEmLFHfxMyHR61746832;     kFHVEmLFHfxMyHR61746832 = kFHVEmLFHfxMyHR8430883;     kFHVEmLFHfxMyHR8430883 = kFHVEmLFHfxMyHR30305788;     kFHVEmLFHfxMyHR30305788 = kFHVEmLFHfxMyHR8895724;     kFHVEmLFHfxMyHR8895724 = kFHVEmLFHfxMyHR20066598;     kFHVEmLFHfxMyHR20066598 = kFHVEmLFHfxMyHR4168409;     kFHVEmLFHfxMyHR4168409 = kFHVEmLFHfxMyHR42893023;}
// Junk Finished

// Junk Code By Peatreat & Thaisen's Gen
void LvMtjxtOCokSIwQS5343361() {     int WfaTnyeHokRwSsP13544690 = -330018331;    int WfaTnyeHokRwSsP56858661 = -385475652;    int WfaTnyeHokRwSsP8179811 = -416989226;    int WfaTnyeHokRwSsP17579816 = -194869783;    int WfaTnyeHokRwSsP31468804 = -435112410;    int WfaTnyeHokRwSsP85528228 = -135473995;    int WfaTnyeHokRwSsP96361249 = -631111674;    int WfaTnyeHokRwSsP70776765 = -742571681;    int WfaTnyeHokRwSsP24618653 = -507619608;    int WfaTnyeHokRwSsP97171922 = 25328745;    int WfaTnyeHokRwSsP31342120 = -776114977;    int WfaTnyeHokRwSsP14558589 = -967867902;    int WfaTnyeHokRwSsP36118779 = -305504797;    int WfaTnyeHokRwSsP72039667 = -618187262;    int WfaTnyeHokRwSsP57076721 = -648623336;    int WfaTnyeHokRwSsP32574249 = -365982867;    int WfaTnyeHokRwSsP66496598 = -380937114;    int WfaTnyeHokRwSsP24624452 = -477612979;    int WfaTnyeHokRwSsP71215456 = -414225936;    int WfaTnyeHokRwSsP76622422 = -576025357;    int WfaTnyeHokRwSsP5558703 = 5822942;    int WfaTnyeHokRwSsP70636324 = -205128093;    int WfaTnyeHokRwSsP27451234 = -350826080;    int WfaTnyeHokRwSsP17478865 = -295327182;    int WfaTnyeHokRwSsP86808588 = -55398574;    int WfaTnyeHokRwSsP1533144 = -643980608;    int WfaTnyeHokRwSsP13743760 = -172975020;    int WfaTnyeHokRwSsP60023768 = -466138503;    int WfaTnyeHokRwSsP5879903 = -458704527;    int WfaTnyeHokRwSsP27707235 = -730830575;    int WfaTnyeHokRwSsP6887452 = -202675248;    int WfaTnyeHokRwSsP43813039 = -214759992;    int WfaTnyeHokRwSsP84718402 = -826135170;    int WfaTnyeHokRwSsP88443813 = -723017241;    int WfaTnyeHokRwSsP25443433 = -613310914;    int WfaTnyeHokRwSsP50305646 = -536608745;    int WfaTnyeHokRwSsP95454897 = -327398759;    int WfaTnyeHokRwSsP61041868 = -924480809;    int WfaTnyeHokRwSsP3013625 = -273409123;    int WfaTnyeHokRwSsP46895622 = -284274469;    int WfaTnyeHokRwSsP94244977 = -799733789;    int WfaTnyeHokRwSsP98985748 = -200431378;    int WfaTnyeHokRwSsP48081144 = -859010249;    int WfaTnyeHokRwSsP964906 = 49921956;    int WfaTnyeHokRwSsP89913847 = -315999273;    int WfaTnyeHokRwSsP90747611 = -802251698;    int WfaTnyeHokRwSsP3540816 = -542201733;    int WfaTnyeHokRwSsP99597213 = 51164315;    int WfaTnyeHokRwSsP79964731 = -547519062;    int WfaTnyeHokRwSsP88472768 = -807794439;    int WfaTnyeHokRwSsP84048003 = -982080426;    int WfaTnyeHokRwSsP55146130 = -47409280;    int WfaTnyeHokRwSsP47722408 = 77092915;    int WfaTnyeHokRwSsP27139298 = -206489866;    int WfaTnyeHokRwSsP99784658 = -879590507;    int WfaTnyeHokRwSsP42908365 = -24890239;    int WfaTnyeHokRwSsP29407428 = 65350428;    int WfaTnyeHokRwSsP90700945 = -21662045;    int WfaTnyeHokRwSsP30771227 = -39471209;    int WfaTnyeHokRwSsP29935661 = -791131802;    int WfaTnyeHokRwSsP71784468 = -962498975;    int WfaTnyeHokRwSsP36337482 = -64973172;    int WfaTnyeHokRwSsP64896863 = -183867154;    int WfaTnyeHokRwSsP96911418 = -776789034;    int WfaTnyeHokRwSsP90284471 = -771996007;    int WfaTnyeHokRwSsP87529080 = -461354986;    int WfaTnyeHokRwSsP29840187 = -41732733;    int WfaTnyeHokRwSsP47674966 = -582487556;    int WfaTnyeHokRwSsP46596234 = 95123651;    int WfaTnyeHokRwSsP6771076 = -12014592;    int WfaTnyeHokRwSsP37119351 = 61415892;    int WfaTnyeHokRwSsP5454731 = -456456306;    int WfaTnyeHokRwSsP21610828 = -104203856;    int WfaTnyeHokRwSsP24319834 = -29951468;    int WfaTnyeHokRwSsP82377444 = -776291568;    int WfaTnyeHokRwSsP6572955 = -793745681;    int WfaTnyeHokRwSsP22555181 = -346117845;    int WfaTnyeHokRwSsP26486328 = -300748036;    int WfaTnyeHokRwSsP27565018 = -979327910;    int WfaTnyeHokRwSsP96060977 = -253146877;    int WfaTnyeHokRwSsP97992327 = -1778876;    int WfaTnyeHokRwSsP14146547 = -124139336;    int WfaTnyeHokRwSsP80059037 = -918619441;    int WfaTnyeHokRwSsP17407134 = -650910089;    int WfaTnyeHokRwSsP43659231 = -748750150;    int WfaTnyeHokRwSsP51741322 = -55265969;    int WfaTnyeHokRwSsP96090631 = -191852908;    int WfaTnyeHokRwSsP57579104 = -519645304;    int WfaTnyeHokRwSsP88659154 = -843426735;    int WfaTnyeHokRwSsP82535068 = -488420676;    int WfaTnyeHokRwSsP20898219 = -501959173;    int WfaTnyeHokRwSsP4753952 = -205736715;    int WfaTnyeHokRwSsP30270642 = -785009600;    int WfaTnyeHokRwSsP73077964 = -482277321;    int WfaTnyeHokRwSsP75111154 = -321775494;    int WfaTnyeHokRwSsP57907496 = -634760618;    int WfaTnyeHokRwSsP34088886 = 83435776;    int WfaTnyeHokRwSsP51169725 = 17778785;    int WfaTnyeHokRwSsP10680435 = -178082037;    int WfaTnyeHokRwSsP2384767 = -330018331;     WfaTnyeHokRwSsP13544690 = WfaTnyeHokRwSsP56858661;     WfaTnyeHokRwSsP56858661 = WfaTnyeHokRwSsP8179811;     WfaTnyeHokRwSsP8179811 = WfaTnyeHokRwSsP17579816;     WfaTnyeHokRwSsP17579816 = WfaTnyeHokRwSsP31468804;     WfaTnyeHokRwSsP31468804 = WfaTnyeHokRwSsP85528228;     WfaTnyeHokRwSsP85528228 = WfaTnyeHokRwSsP96361249;     WfaTnyeHokRwSsP96361249 = WfaTnyeHokRwSsP70776765;     WfaTnyeHokRwSsP70776765 = WfaTnyeHokRwSsP24618653;     WfaTnyeHokRwSsP24618653 = WfaTnyeHokRwSsP97171922;     WfaTnyeHokRwSsP97171922 = WfaTnyeHokRwSsP31342120;     WfaTnyeHokRwSsP31342120 = WfaTnyeHokRwSsP14558589;     WfaTnyeHokRwSsP14558589 = WfaTnyeHokRwSsP36118779;     WfaTnyeHokRwSsP36118779 = WfaTnyeHokRwSsP72039667;     WfaTnyeHokRwSsP72039667 = WfaTnyeHokRwSsP57076721;     WfaTnyeHokRwSsP57076721 = WfaTnyeHokRwSsP32574249;     WfaTnyeHokRwSsP32574249 = WfaTnyeHokRwSsP66496598;     WfaTnyeHokRwSsP66496598 = WfaTnyeHokRwSsP24624452;     WfaTnyeHokRwSsP24624452 = WfaTnyeHokRwSsP71215456;     WfaTnyeHokRwSsP71215456 = WfaTnyeHokRwSsP76622422;     WfaTnyeHokRwSsP76622422 = WfaTnyeHokRwSsP5558703;     WfaTnyeHokRwSsP5558703 = WfaTnyeHokRwSsP70636324;     WfaTnyeHokRwSsP70636324 = WfaTnyeHokRwSsP27451234;     WfaTnyeHokRwSsP27451234 = WfaTnyeHokRwSsP17478865;     WfaTnyeHokRwSsP17478865 = WfaTnyeHokRwSsP86808588;     WfaTnyeHokRwSsP86808588 = WfaTnyeHokRwSsP1533144;     WfaTnyeHokRwSsP1533144 = WfaTnyeHokRwSsP13743760;     WfaTnyeHokRwSsP13743760 = WfaTnyeHokRwSsP60023768;     WfaTnyeHokRwSsP60023768 = WfaTnyeHokRwSsP5879903;     WfaTnyeHokRwSsP5879903 = WfaTnyeHokRwSsP27707235;     WfaTnyeHokRwSsP27707235 = WfaTnyeHokRwSsP6887452;     WfaTnyeHokRwSsP6887452 = WfaTnyeHokRwSsP43813039;     WfaTnyeHokRwSsP43813039 = WfaTnyeHokRwSsP84718402;     WfaTnyeHokRwSsP84718402 = WfaTnyeHokRwSsP88443813;     WfaTnyeHokRwSsP88443813 = WfaTnyeHokRwSsP25443433;     WfaTnyeHokRwSsP25443433 = WfaTnyeHokRwSsP50305646;     WfaTnyeHokRwSsP50305646 = WfaTnyeHokRwSsP95454897;     WfaTnyeHokRwSsP95454897 = WfaTnyeHokRwSsP61041868;     WfaTnyeHokRwSsP61041868 = WfaTnyeHokRwSsP3013625;     WfaTnyeHokRwSsP3013625 = WfaTnyeHokRwSsP46895622;     WfaTnyeHokRwSsP46895622 = WfaTnyeHokRwSsP94244977;     WfaTnyeHokRwSsP94244977 = WfaTnyeHokRwSsP98985748;     WfaTnyeHokRwSsP98985748 = WfaTnyeHokRwSsP48081144;     WfaTnyeHokRwSsP48081144 = WfaTnyeHokRwSsP964906;     WfaTnyeHokRwSsP964906 = WfaTnyeHokRwSsP89913847;     WfaTnyeHokRwSsP89913847 = WfaTnyeHokRwSsP90747611;     WfaTnyeHokRwSsP90747611 = WfaTnyeHokRwSsP3540816;     WfaTnyeHokRwSsP3540816 = WfaTnyeHokRwSsP99597213;     WfaTnyeHokRwSsP99597213 = WfaTnyeHokRwSsP79964731;     WfaTnyeHokRwSsP79964731 = WfaTnyeHokRwSsP88472768;     WfaTnyeHokRwSsP88472768 = WfaTnyeHokRwSsP84048003;     WfaTnyeHokRwSsP84048003 = WfaTnyeHokRwSsP55146130;     WfaTnyeHokRwSsP55146130 = WfaTnyeHokRwSsP47722408;     WfaTnyeHokRwSsP47722408 = WfaTnyeHokRwSsP27139298;     WfaTnyeHokRwSsP27139298 = WfaTnyeHokRwSsP99784658;     WfaTnyeHokRwSsP99784658 = WfaTnyeHokRwSsP42908365;     WfaTnyeHokRwSsP42908365 = WfaTnyeHokRwSsP29407428;     WfaTnyeHokRwSsP29407428 = WfaTnyeHokRwSsP90700945;     WfaTnyeHokRwSsP90700945 = WfaTnyeHokRwSsP30771227;     WfaTnyeHokRwSsP30771227 = WfaTnyeHokRwSsP29935661;     WfaTnyeHokRwSsP29935661 = WfaTnyeHokRwSsP71784468;     WfaTnyeHokRwSsP71784468 = WfaTnyeHokRwSsP36337482;     WfaTnyeHokRwSsP36337482 = WfaTnyeHokRwSsP64896863;     WfaTnyeHokRwSsP64896863 = WfaTnyeHokRwSsP96911418;     WfaTnyeHokRwSsP96911418 = WfaTnyeHokRwSsP90284471;     WfaTnyeHokRwSsP90284471 = WfaTnyeHokRwSsP87529080;     WfaTnyeHokRwSsP87529080 = WfaTnyeHokRwSsP29840187;     WfaTnyeHokRwSsP29840187 = WfaTnyeHokRwSsP47674966;     WfaTnyeHokRwSsP47674966 = WfaTnyeHokRwSsP46596234;     WfaTnyeHokRwSsP46596234 = WfaTnyeHokRwSsP6771076;     WfaTnyeHokRwSsP6771076 = WfaTnyeHokRwSsP37119351;     WfaTnyeHokRwSsP37119351 = WfaTnyeHokRwSsP5454731;     WfaTnyeHokRwSsP5454731 = WfaTnyeHokRwSsP21610828;     WfaTnyeHokRwSsP21610828 = WfaTnyeHokRwSsP24319834;     WfaTnyeHokRwSsP24319834 = WfaTnyeHokRwSsP82377444;     WfaTnyeHokRwSsP82377444 = WfaTnyeHokRwSsP6572955;     WfaTnyeHokRwSsP6572955 = WfaTnyeHokRwSsP22555181;     WfaTnyeHokRwSsP22555181 = WfaTnyeHokRwSsP26486328;     WfaTnyeHokRwSsP26486328 = WfaTnyeHokRwSsP27565018;     WfaTnyeHokRwSsP27565018 = WfaTnyeHokRwSsP96060977;     WfaTnyeHokRwSsP96060977 = WfaTnyeHokRwSsP97992327;     WfaTnyeHokRwSsP97992327 = WfaTnyeHokRwSsP14146547;     WfaTnyeHokRwSsP14146547 = WfaTnyeHokRwSsP80059037;     WfaTnyeHokRwSsP80059037 = WfaTnyeHokRwSsP17407134;     WfaTnyeHokRwSsP17407134 = WfaTnyeHokRwSsP43659231;     WfaTnyeHokRwSsP43659231 = WfaTnyeHokRwSsP51741322;     WfaTnyeHokRwSsP51741322 = WfaTnyeHokRwSsP96090631;     WfaTnyeHokRwSsP96090631 = WfaTnyeHokRwSsP57579104;     WfaTnyeHokRwSsP57579104 = WfaTnyeHokRwSsP88659154;     WfaTnyeHokRwSsP88659154 = WfaTnyeHokRwSsP82535068;     WfaTnyeHokRwSsP82535068 = WfaTnyeHokRwSsP20898219;     WfaTnyeHokRwSsP20898219 = WfaTnyeHokRwSsP4753952;     WfaTnyeHokRwSsP4753952 = WfaTnyeHokRwSsP30270642;     WfaTnyeHokRwSsP30270642 = WfaTnyeHokRwSsP73077964;     WfaTnyeHokRwSsP73077964 = WfaTnyeHokRwSsP75111154;     WfaTnyeHokRwSsP75111154 = WfaTnyeHokRwSsP57907496;     WfaTnyeHokRwSsP57907496 = WfaTnyeHokRwSsP34088886;     WfaTnyeHokRwSsP34088886 = WfaTnyeHokRwSsP51169725;     WfaTnyeHokRwSsP51169725 = WfaTnyeHokRwSsP10680435;     WfaTnyeHokRwSsP10680435 = WfaTnyeHokRwSsP2384767;     WfaTnyeHokRwSsP2384767 = WfaTnyeHokRwSsP13544690;}
// Junk Finished

// Junk Code By Peatreat & Thaisen's Gen
void TvsapoXRjnvQwnex12002106() {     int uMoWDYpUtpieNAs13538219 = -202542474;    int uMoWDYpUtpieNAs95430977 = -428070783;    int uMoWDYpUtpieNAs9493023 = -374117839;    int uMoWDYpUtpieNAs2659677 = -15410610;    int uMoWDYpUtpieNAs40308906 = -903467739;    int uMoWDYpUtpieNAs20333661 = -934906807;    int uMoWDYpUtpieNAs64908586 = -335648141;    int uMoWDYpUtpieNAs39990816 = -446749013;    int uMoWDYpUtpieNAs7358817 = -221265679;    int uMoWDYpUtpieNAs93811436 = -711478126;    int uMoWDYpUtpieNAs51662725 = -735587050;    int uMoWDYpUtpieNAs28850266 = -703346274;    int uMoWDYpUtpieNAs67971692 = -327163272;    int uMoWDYpUtpieNAs25883887 = -963340315;    int uMoWDYpUtpieNAs2248831 = -611336207;    int uMoWDYpUtpieNAs7138797 = -698778518;    int uMoWDYpUtpieNAs22349358 = -200551684;    int uMoWDYpUtpieNAs47624985 = -874684604;    int uMoWDYpUtpieNAs91643726 = 67791798;    int uMoWDYpUtpieNAs48696185 = -88049808;    int uMoWDYpUtpieNAs85228126 = -804643437;    int uMoWDYpUtpieNAs48932591 = -715119976;    int uMoWDYpUtpieNAs64329630 = -809783922;    int uMoWDYpUtpieNAs61128973 = -176247704;    int uMoWDYpUtpieNAs15251279 = -482092440;    int uMoWDYpUtpieNAs18107221 = -552186533;    int uMoWDYpUtpieNAs83965341 = -473774733;    int uMoWDYpUtpieNAs95427293 = 24894503;    int uMoWDYpUtpieNAs18558349 = -4774827;    int uMoWDYpUtpieNAs3797105 = -406261949;    int uMoWDYpUtpieNAs40300130 = -589768483;    int uMoWDYpUtpieNAs28536897 = -606581750;    int uMoWDYpUtpieNAs70170825 = 71920618;    int uMoWDYpUtpieNAs88726650 = -599698974;    int uMoWDYpUtpieNAs7801472 = -872624836;    int uMoWDYpUtpieNAs93133836 = -616374472;    int uMoWDYpUtpieNAs7396265 = -427052137;    int uMoWDYpUtpieNAs30959548 = -919743491;    int uMoWDYpUtpieNAs38327222 = -986090526;    int uMoWDYpUtpieNAs18768515 = -993706155;    int uMoWDYpUtpieNAs82100645 = -887677687;    int uMoWDYpUtpieNAs84683409 = -394223440;    int uMoWDYpUtpieNAs8364504 = -318140501;    int uMoWDYpUtpieNAs60196804 = -607042487;    int uMoWDYpUtpieNAs73931899 = -789258673;    int uMoWDYpUtpieNAs31175579 = -614296350;    int uMoWDYpUtpieNAs31238608 = 76517645;    int uMoWDYpUtpieNAs74980794 = -557597040;    int uMoWDYpUtpieNAs98623120 = -111282899;    int uMoWDYpUtpieNAs8518192 = -394819330;    int uMoWDYpUtpieNAs97307373 = -560635433;    int uMoWDYpUtpieNAs69693387 = -429555024;    int uMoWDYpUtpieNAs23170756 = -914961096;    int uMoWDYpUtpieNAs37389569 = -738409718;    int uMoWDYpUtpieNAs26773979 = -636489779;    int uMoWDYpUtpieNAs64605628 = -487422499;    int uMoWDYpUtpieNAs31101347 = -618286862;    int uMoWDYpUtpieNAs48364049 = -97870136;    int uMoWDYpUtpieNAs87408398 = -533318171;    int uMoWDYpUtpieNAs22201686 = -251281207;    int uMoWDYpUtpieNAs36368319 = -361132074;    int uMoWDYpUtpieNAs69481292 = -260542644;    int uMoWDYpUtpieNAs21432467 = -341974187;    int uMoWDYpUtpieNAs3561713 = -815003731;    int uMoWDYpUtpieNAs53511306 = -21709644;    int uMoWDYpUtpieNAs23125829 = -29005300;    int uMoWDYpUtpieNAs58679440 = -675266892;    int uMoWDYpUtpieNAs79245042 = -727464298;    int uMoWDYpUtpieNAs18082416 = 9284520;    int uMoWDYpUtpieNAs9114995 = -994961735;    int uMoWDYpUtpieNAs99742531 = -171726381;    int uMoWDYpUtpieNAs91389810 = -280808194;    int uMoWDYpUtpieNAs9297764 = -888594078;    int uMoWDYpUtpieNAs72875211 = 61497952;    int uMoWDYpUtpieNAs66595539 = -200372121;    int uMoWDYpUtpieNAs544718 = -310419998;    int uMoWDYpUtpieNAs40568087 = -296979476;    int uMoWDYpUtpieNAs4132827 = -102741435;    int uMoWDYpUtpieNAs87197073 = -386989032;    int uMoWDYpUtpieNAs84075699 = -867796090;    int uMoWDYpUtpieNAs86868613 = -528704178;    int uMoWDYpUtpieNAs8984548 = -916177694;    int uMoWDYpUtpieNAs96804172 = -863822599;    int uMoWDYpUtpieNAs10040158 = -609955497;    int uMoWDYpUtpieNAs6489731 = -845626516;    int uMoWDYpUtpieNAs70606743 = -60213460;    int uMoWDYpUtpieNAs5366141 = -691620655;    int uMoWDYpUtpieNAs32781257 = -189669665;    int uMoWDYpUtpieNAs61952671 = -963209195;    int uMoWDYpUtpieNAs43195844 = -285202337;    int uMoWDYpUtpieNAs62032489 = -998087611;    int uMoWDYpUtpieNAs59032216 = -229182002;    int uMoWDYpUtpieNAs43551150 = -286425320;    int uMoWDYpUtpieNAs16125536 = -634809320;    int uMoWDYpUtpieNAs82400195 = -532574081;    int uMoWDYpUtpieNAs12619353 = -527135044;    int uMoWDYpUtpieNAs63250943 = 47750747;    int uMoWDYpUtpieNAs4802792 = -503136770;    int uMoWDYpUtpieNAs6685498 = -485332844;    int uMoWDYpUtpieNAs50806071 = -202542474;     uMoWDYpUtpieNAs13538219 = uMoWDYpUtpieNAs95430977;     uMoWDYpUtpieNAs95430977 = uMoWDYpUtpieNAs9493023;     uMoWDYpUtpieNAs9493023 = uMoWDYpUtpieNAs2659677;     uMoWDYpUtpieNAs2659677 = uMoWDYpUtpieNAs40308906;     uMoWDYpUtpieNAs40308906 = uMoWDYpUtpieNAs20333661;     uMoWDYpUtpieNAs20333661 = uMoWDYpUtpieNAs64908586;     uMoWDYpUtpieNAs64908586 = uMoWDYpUtpieNAs39990816;     uMoWDYpUtpieNAs39990816 = uMoWDYpUtpieNAs7358817;     uMoWDYpUtpieNAs7358817 = uMoWDYpUtpieNAs93811436;     uMoWDYpUtpieNAs93811436 = uMoWDYpUtpieNAs51662725;     uMoWDYpUtpieNAs51662725 = uMoWDYpUtpieNAs28850266;     uMoWDYpUtpieNAs28850266 = uMoWDYpUtpieNAs67971692;     uMoWDYpUtpieNAs67971692 = uMoWDYpUtpieNAs25883887;     uMoWDYpUtpieNAs25883887 = uMoWDYpUtpieNAs2248831;     uMoWDYpUtpieNAs2248831 = uMoWDYpUtpieNAs7138797;     uMoWDYpUtpieNAs7138797 = uMoWDYpUtpieNAs22349358;     uMoWDYpUtpieNAs22349358 = uMoWDYpUtpieNAs47624985;     uMoWDYpUtpieNAs47624985 = uMoWDYpUtpieNAs91643726;     uMoWDYpUtpieNAs91643726 = uMoWDYpUtpieNAs48696185;     uMoWDYpUtpieNAs48696185 = uMoWDYpUtpieNAs85228126;     uMoWDYpUtpieNAs85228126 = uMoWDYpUtpieNAs48932591;     uMoWDYpUtpieNAs48932591 = uMoWDYpUtpieNAs64329630;     uMoWDYpUtpieNAs64329630 = uMoWDYpUtpieNAs61128973;     uMoWDYpUtpieNAs61128973 = uMoWDYpUtpieNAs15251279;     uMoWDYpUtpieNAs15251279 = uMoWDYpUtpieNAs18107221;     uMoWDYpUtpieNAs18107221 = uMoWDYpUtpieNAs83965341;     uMoWDYpUtpieNAs83965341 = uMoWDYpUtpieNAs95427293;     uMoWDYpUtpieNAs95427293 = uMoWDYpUtpieNAs18558349;     uMoWDYpUtpieNAs18558349 = uMoWDYpUtpieNAs3797105;     uMoWDYpUtpieNAs3797105 = uMoWDYpUtpieNAs40300130;     uMoWDYpUtpieNAs40300130 = uMoWDYpUtpieNAs28536897;     uMoWDYpUtpieNAs28536897 = uMoWDYpUtpieNAs70170825;     uMoWDYpUtpieNAs70170825 = uMoWDYpUtpieNAs88726650;     uMoWDYpUtpieNAs88726650 = uMoWDYpUtpieNAs7801472;     uMoWDYpUtpieNAs7801472 = uMoWDYpUtpieNAs93133836;     uMoWDYpUtpieNAs93133836 = uMoWDYpUtpieNAs7396265;     uMoWDYpUtpieNAs7396265 = uMoWDYpUtpieNAs30959548;     uMoWDYpUtpieNAs30959548 = uMoWDYpUtpieNAs38327222;     uMoWDYpUtpieNAs38327222 = uMoWDYpUtpieNAs18768515;     uMoWDYpUtpieNAs18768515 = uMoWDYpUtpieNAs82100645;     uMoWDYpUtpieNAs82100645 = uMoWDYpUtpieNAs84683409;     uMoWDYpUtpieNAs84683409 = uMoWDYpUtpieNAs8364504;     uMoWDYpUtpieNAs8364504 = uMoWDYpUtpieNAs60196804;     uMoWDYpUtpieNAs60196804 = uMoWDYpUtpieNAs73931899;     uMoWDYpUtpieNAs73931899 = uMoWDYpUtpieNAs31175579;     uMoWDYpUtpieNAs31175579 = uMoWDYpUtpieNAs31238608;     uMoWDYpUtpieNAs31238608 = uMoWDYpUtpieNAs74980794;     uMoWDYpUtpieNAs74980794 = uMoWDYpUtpieNAs98623120;     uMoWDYpUtpieNAs98623120 = uMoWDYpUtpieNAs8518192;     uMoWDYpUtpieNAs8518192 = uMoWDYpUtpieNAs97307373;     uMoWDYpUtpieNAs97307373 = uMoWDYpUtpieNAs69693387;     uMoWDYpUtpieNAs69693387 = uMoWDYpUtpieNAs23170756;     uMoWDYpUtpieNAs23170756 = uMoWDYpUtpieNAs37389569;     uMoWDYpUtpieNAs37389569 = uMoWDYpUtpieNAs26773979;     uMoWDYpUtpieNAs26773979 = uMoWDYpUtpieNAs64605628;     uMoWDYpUtpieNAs64605628 = uMoWDYpUtpieNAs31101347;     uMoWDYpUtpieNAs31101347 = uMoWDYpUtpieNAs48364049;     uMoWDYpUtpieNAs48364049 = uMoWDYpUtpieNAs87408398;     uMoWDYpUtpieNAs87408398 = uMoWDYpUtpieNAs22201686;     uMoWDYpUtpieNAs22201686 = uMoWDYpUtpieNAs36368319;     uMoWDYpUtpieNAs36368319 = uMoWDYpUtpieNAs69481292;     uMoWDYpUtpieNAs69481292 = uMoWDYpUtpieNAs21432467;     uMoWDYpUtpieNAs21432467 = uMoWDYpUtpieNAs3561713;     uMoWDYpUtpieNAs3561713 = uMoWDYpUtpieNAs53511306;     uMoWDYpUtpieNAs53511306 = uMoWDYpUtpieNAs23125829;     uMoWDYpUtpieNAs23125829 = uMoWDYpUtpieNAs58679440;     uMoWDYpUtpieNAs58679440 = uMoWDYpUtpieNAs79245042;     uMoWDYpUtpieNAs79245042 = uMoWDYpUtpieNAs18082416;     uMoWDYpUtpieNAs18082416 = uMoWDYpUtpieNAs9114995;     uMoWDYpUtpieNAs9114995 = uMoWDYpUtpieNAs99742531;     uMoWDYpUtpieNAs99742531 = uMoWDYpUtpieNAs91389810;     uMoWDYpUtpieNAs91389810 = uMoWDYpUtpieNAs9297764;     uMoWDYpUtpieNAs9297764 = uMoWDYpUtpieNAs72875211;     uMoWDYpUtpieNAs72875211 = uMoWDYpUtpieNAs66595539;     uMoWDYpUtpieNAs66595539 = uMoWDYpUtpieNAs544718;     uMoWDYpUtpieNAs544718 = uMoWDYpUtpieNAs40568087;     uMoWDYpUtpieNAs40568087 = uMoWDYpUtpieNAs4132827;     uMoWDYpUtpieNAs4132827 = uMoWDYpUtpieNAs87197073;     uMoWDYpUtpieNAs87197073 = uMoWDYpUtpieNAs84075699;     uMoWDYpUtpieNAs84075699 = uMoWDYpUtpieNAs86868613;     uMoWDYpUtpieNAs86868613 = uMoWDYpUtpieNAs8984548;     uMoWDYpUtpieNAs8984548 = uMoWDYpUtpieNAs96804172;     uMoWDYpUtpieNAs96804172 = uMoWDYpUtpieNAs10040158;     uMoWDYpUtpieNAs10040158 = uMoWDYpUtpieNAs6489731;     uMoWDYpUtpieNAs6489731 = uMoWDYpUtpieNAs70606743;     uMoWDYpUtpieNAs70606743 = uMoWDYpUtpieNAs5366141;     uMoWDYpUtpieNAs5366141 = uMoWDYpUtpieNAs32781257;     uMoWDYpUtpieNAs32781257 = uMoWDYpUtpieNAs61952671;     uMoWDYpUtpieNAs61952671 = uMoWDYpUtpieNAs43195844;     uMoWDYpUtpieNAs43195844 = uMoWDYpUtpieNAs62032489;     uMoWDYpUtpieNAs62032489 = uMoWDYpUtpieNAs59032216;     uMoWDYpUtpieNAs59032216 = uMoWDYpUtpieNAs43551150;     uMoWDYpUtpieNAs43551150 = uMoWDYpUtpieNAs16125536;     uMoWDYpUtpieNAs16125536 = uMoWDYpUtpieNAs82400195;     uMoWDYpUtpieNAs82400195 = uMoWDYpUtpieNAs12619353;     uMoWDYpUtpieNAs12619353 = uMoWDYpUtpieNAs63250943;     uMoWDYpUtpieNAs63250943 = uMoWDYpUtpieNAs4802792;     uMoWDYpUtpieNAs4802792 = uMoWDYpUtpieNAs6685498;     uMoWDYpUtpieNAs6685498 = uMoWDYpUtpieNAs50806071;     uMoWDYpUtpieNAs50806071 = uMoWDYpUtpieNAs13538219;}
// Junk Finished

// Junk Code By Peatreat & Thaisen's Gen
void qLOMoUhZxyaudKaj66418318() {     int nYIRikbHqnCzKYu84189885 = -820691603;    int nYIRikbHqnCzKYu64104495 = -299714076;    int nYIRikbHqnCzKYu89461941 = -816412299;    int nYIRikbHqnCzKYu42590963 = -205549002;    int nYIRikbHqnCzKYu61759228 = -686874078;    int nYIRikbHqnCzKYu10333424 = -636487958;    int nYIRikbHqnCzKYu83287481 = -334423850;    int nYIRikbHqnCzKYu96833196 = -793566088;    int nYIRikbHqnCzKYu53223871 = -314613007;    int nYIRikbHqnCzKYu16777897 = 87426523;    int nYIRikbHqnCzKYu93911885 = -770488978;    int nYIRikbHqnCzKYu74396837 = 80889943;    int nYIRikbHqnCzKYu47799080 = -236700330;    int nYIRikbHqnCzKYu9730921 = -764368524;    int nYIRikbHqnCzKYu84968849 = -761086191;    int nYIRikbHqnCzKYu39667436 = -268762104;    int nYIRikbHqnCzKYu16202185 = -109736814;    int nYIRikbHqnCzKYu67918479 = -863850796;    int nYIRikbHqnCzKYu73351507 = -639396145;    int nYIRikbHqnCzKYu7482895 = -630416113;    int nYIRikbHqnCzKYu40363881 = -713322572;    int nYIRikbHqnCzKYu10328356 = 53594718;    int nYIRikbHqnCzKYu66273439 = -277627066;    int nYIRikbHqnCzKYu94978510 = -868648610;    int nYIRikbHqnCzKYu79375667 = -157997115;    int nYIRikbHqnCzKYu48704801 = -650470981;    int nYIRikbHqnCzKYu92226127 = -868952508;    int nYIRikbHqnCzKYu3209519 = -480229258;    int nYIRikbHqnCzKYu50700837 = -299133059;    int nYIRikbHqnCzKYu42969200 = -964657019;    int nYIRikbHqnCzKYu47372127 = -338202168;    int nYIRikbHqnCzKYu73048357 = -934878916;    int nYIRikbHqnCzKYu53116186 = -975040994;    int nYIRikbHqnCzKYu54091625 = -163228658;    int nYIRikbHqnCzKYu85825707 = -719571618;    int nYIRikbHqnCzKYu53127095 = -695601039;    int nYIRikbHqnCzKYu51187751 = -615645345;    int nYIRikbHqnCzKYu83752036 = -703856595;    int nYIRikbHqnCzKYu16860511 = -941819175;    int nYIRikbHqnCzKYu13871814 = -511314517;    int nYIRikbHqnCzKYu42173929 = -171696566;    int nYIRikbHqnCzKYu13166401 = -163612530;    int nYIRikbHqnCzKYu57331464 = -304894555;    int nYIRikbHqnCzKYu66705104 = -679017459;    int nYIRikbHqnCzKYu69885958 = 2094931;    int nYIRikbHqnCzKYu37581414 = -372633524;    int nYIRikbHqnCzKYu57369811 = -39552154;    int nYIRikbHqnCzKYu40595004 = 90272664;    int nYIRikbHqnCzKYu4064573 = -97046893;    int nYIRikbHqnCzKYu16945678 = -677828204;    int nYIRikbHqnCzKYu65496698 = -107207554;    int nYIRikbHqnCzKYu11564892 = -398774750;    int nYIRikbHqnCzKYu78246413 = -842759135;    int nYIRikbHqnCzKYu27616874 = -299291214;    int nYIRikbHqnCzKYu26054438 = -747430699;    int nYIRikbHqnCzKYu73861530 = -774286322;    int nYIRikbHqnCzKYu97831056 = 77912990;    int nYIRikbHqnCzKYu94483431 = -947763689;    int nYIRikbHqnCzKYu63215296 = 52448112;    int nYIRikbHqnCzKYu13054427 = 63596903;    int nYIRikbHqnCzKYu18107296 = -767535451;    int nYIRikbHqnCzKYu80077962 = -854194592;    int nYIRikbHqnCzKYu46132359 = -394433029;    int nYIRikbHqnCzKYu10254671 = -349955988;    int nYIRikbHqnCzKYu69405770 = -574371309;    int nYIRikbHqnCzKYu20863529 = -835610063;    int nYIRikbHqnCzKYu21280651 = 55930936;    int nYIRikbHqnCzKYu93707454 = 26528328;    int nYIRikbHqnCzKYu23905213 = 55203094;    int nYIRikbHqnCzKYu31841755 = 34514847;    int nYIRikbHqnCzKYu88479685 = -653116760;    int nYIRikbHqnCzKYu32450149 = -405880219;    int nYIRikbHqnCzKYu51057969 = -922031621;    int nYIRikbHqnCzKYu59479694 = -28081628;    int nYIRikbHqnCzKYu65308965 = -358719548;    int nYIRikbHqnCzKYu27197481 = -449710043;    int nYIRikbHqnCzKYu52996891 = -641510727;    int nYIRikbHqnCzKYu99568334 = -598609608;    int nYIRikbHqnCzKYu25092552 = -770743542;    int nYIRikbHqnCzKYu41794253 = -785363592;    int nYIRikbHqnCzKYu91334989 = -510918828;    int nYIRikbHqnCzKYu51631123 = -859225173;    int nYIRikbHqnCzKYu99144945 = -283182366;    int nYIRikbHqnCzKYu33755159 = -621304856;    int nYIRikbHqnCzKYu77472502 = -757449466;    int nYIRikbHqnCzKYu35807235 = -939427419;    int nYIRikbHqnCzKYu94801944 = 7880218;    int nYIRikbHqnCzKYu25499313 = -575749780;    int nYIRikbHqnCzKYu28037188 = -415797960;    int nYIRikbHqnCzKYu11964178 = -945285297;    int nYIRikbHqnCzKYu55296038 = -673514029;    int nYIRikbHqnCzKYu56704320 = -667881656;    int nYIRikbHqnCzKYu20536740 = -656304708;    int nYIRikbHqnCzKYu3806084 = -905416079;    int nYIRikbHqnCzKYu95764517 = -743779067;    int nYIRikbHqnCzKYu62095967 = -317501974;    int nYIRikbHqnCzKYu67034041 = -769179501;    int nYIRikbHqnCzKYu47076793 = -954938567;    int nYIRikbHqnCzKYu97299334 = -4646151;    int nYIRikbHqnCzKYu49022430 = -820691603;     nYIRikbHqnCzKYu84189885 = nYIRikbHqnCzKYu64104495;     nYIRikbHqnCzKYu64104495 = nYIRikbHqnCzKYu89461941;     nYIRikbHqnCzKYu89461941 = nYIRikbHqnCzKYu42590963;     nYIRikbHqnCzKYu42590963 = nYIRikbHqnCzKYu61759228;     nYIRikbHqnCzKYu61759228 = nYIRikbHqnCzKYu10333424;     nYIRikbHqnCzKYu10333424 = nYIRikbHqnCzKYu83287481;     nYIRikbHqnCzKYu83287481 = nYIRikbHqnCzKYu96833196;     nYIRikbHqnCzKYu96833196 = nYIRikbHqnCzKYu53223871;     nYIRikbHqnCzKYu53223871 = nYIRikbHqnCzKYu16777897;     nYIRikbHqnCzKYu16777897 = nYIRikbHqnCzKYu93911885;     nYIRikbHqnCzKYu93911885 = nYIRikbHqnCzKYu74396837;     nYIRikbHqnCzKYu74396837 = nYIRikbHqnCzKYu47799080;     nYIRikbHqnCzKYu47799080 = nYIRikbHqnCzKYu9730921;     nYIRikbHqnCzKYu9730921 = nYIRikbHqnCzKYu84968849;     nYIRikbHqnCzKYu84968849 = nYIRikbHqnCzKYu39667436;     nYIRikbHqnCzKYu39667436 = nYIRikbHqnCzKYu16202185;     nYIRikbHqnCzKYu16202185 = nYIRikbHqnCzKYu67918479;     nYIRikbHqnCzKYu67918479 = nYIRikbHqnCzKYu73351507;     nYIRikbHqnCzKYu73351507 = nYIRikbHqnCzKYu7482895;     nYIRikbHqnCzKYu7482895 = nYIRikbHqnCzKYu40363881;     nYIRikbHqnCzKYu40363881 = nYIRikbHqnCzKYu10328356;     nYIRikbHqnCzKYu10328356 = nYIRikbHqnCzKYu66273439;     nYIRikbHqnCzKYu66273439 = nYIRikbHqnCzKYu94978510;     nYIRikbHqnCzKYu94978510 = nYIRikbHqnCzKYu79375667;     nYIRikbHqnCzKYu79375667 = nYIRikbHqnCzKYu48704801;     nYIRikbHqnCzKYu48704801 = nYIRikbHqnCzKYu92226127;     nYIRikbHqnCzKYu92226127 = nYIRikbHqnCzKYu3209519;     nYIRikbHqnCzKYu3209519 = nYIRikbHqnCzKYu50700837;     nYIRikbHqnCzKYu50700837 = nYIRikbHqnCzKYu42969200;     nYIRikbHqnCzKYu42969200 = nYIRikbHqnCzKYu47372127;     nYIRikbHqnCzKYu47372127 = nYIRikbHqnCzKYu73048357;     nYIRikbHqnCzKYu73048357 = nYIRikbHqnCzKYu53116186;     nYIRikbHqnCzKYu53116186 = nYIRikbHqnCzKYu54091625;     nYIRikbHqnCzKYu54091625 = nYIRikbHqnCzKYu85825707;     nYIRikbHqnCzKYu85825707 = nYIRikbHqnCzKYu53127095;     nYIRikbHqnCzKYu53127095 = nYIRikbHqnCzKYu51187751;     nYIRikbHqnCzKYu51187751 = nYIRikbHqnCzKYu83752036;     nYIRikbHqnCzKYu83752036 = nYIRikbHqnCzKYu16860511;     nYIRikbHqnCzKYu16860511 = nYIRikbHqnCzKYu13871814;     nYIRikbHqnCzKYu13871814 = nYIRikbHqnCzKYu42173929;     nYIRikbHqnCzKYu42173929 = nYIRikbHqnCzKYu13166401;     nYIRikbHqnCzKYu13166401 = nYIRikbHqnCzKYu57331464;     nYIRikbHqnCzKYu57331464 = nYIRikbHqnCzKYu66705104;     nYIRikbHqnCzKYu66705104 = nYIRikbHqnCzKYu69885958;     nYIRikbHqnCzKYu69885958 = nYIRikbHqnCzKYu37581414;     nYIRikbHqnCzKYu37581414 = nYIRikbHqnCzKYu57369811;     nYIRikbHqnCzKYu57369811 = nYIRikbHqnCzKYu40595004;     nYIRikbHqnCzKYu40595004 = nYIRikbHqnCzKYu4064573;     nYIRikbHqnCzKYu4064573 = nYIRikbHqnCzKYu16945678;     nYIRikbHqnCzKYu16945678 = nYIRikbHqnCzKYu65496698;     nYIRikbHqnCzKYu65496698 = nYIRikbHqnCzKYu11564892;     nYIRikbHqnCzKYu11564892 = nYIRikbHqnCzKYu78246413;     nYIRikbHqnCzKYu78246413 = nYIRikbHqnCzKYu27616874;     nYIRikbHqnCzKYu27616874 = nYIRikbHqnCzKYu26054438;     nYIRikbHqnCzKYu26054438 = nYIRikbHqnCzKYu73861530;     nYIRikbHqnCzKYu73861530 = nYIRikbHqnCzKYu97831056;     nYIRikbHqnCzKYu97831056 = nYIRikbHqnCzKYu94483431;     nYIRikbHqnCzKYu94483431 = nYIRikbHqnCzKYu63215296;     nYIRikbHqnCzKYu63215296 = nYIRikbHqnCzKYu13054427;     nYIRikbHqnCzKYu13054427 = nYIRikbHqnCzKYu18107296;     nYIRikbHqnCzKYu18107296 = nYIRikbHqnCzKYu80077962;     nYIRikbHqnCzKYu80077962 = nYIRikbHqnCzKYu46132359;     nYIRikbHqnCzKYu46132359 = nYIRikbHqnCzKYu10254671;     nYIRikbHqnCzKYu10254671 = nYIRikbHqnCzKYu69405770;     nYIRikbHqnCzKYu69405770 = nYIRikbHqnCzKYu20863529;     nYIRikbHqnCzKYu20863529 = nYIRikbHqnCzKYu21280651;     nYIRikbHqnCzKYu21280651 = nYIRikbHqnCzKYu93707454;     nYIRikbHqnCzKYu93707454 = nYIRikbHqnCzKYu23905213;     nYIRikbHqnCzKYu23905213 = nYIRikbHqnCzKYu31841755;     nYIRikbHqnCzKYu31841755 = nYIRikbHqnCzKYu88479685;     nYIRikbHqnCzKYu88479685 = nYIRikbHqnCzKYu32450149;     nYIRikbHqnCzKYu32450149 = nYIRikbHqnCzKYu51057969;     nYIRikbHqnCzKYu51057969 = nYIRikbHqnCzKYu59479694;     nYIRikbHqnCzKYu59479694 = nYIRikbHqnCzKYu65308965;     nYIRikbHqnCzKYu65308965 = nYIRikbHqnCzKYu27197481;     nYIRikbHqnCzKYu27197481 = nYIRikbHqnCzKYu52996891;     nYIRikbHqnCzKYu52996891 = nYIRikbHqnCzKYu99568334;     nYIRikbHqnCzKYu99568334 = nYIRikbHqnCzKYu25092552;     nYIRikbHqnCzKYu25092552 = nYIRikbHqnCzKYu41794253;     nYIRikbHqnCzKYu41794253 = nYIRikbHqnCzKYu91334989;     nYIRikbHqnCzKYu91334989 = nYIRikbHqnCzKYu51631123;     nYIRikbHqnCzKYu51631123 = nYIRikbHqnCzKYu99144945;     nYIRikbHqnCzKYu99144945 = nYIRikbHqnCzKYu33755159;     nYIRikbHqnCzKYu33755159 = nYIRikbHqnCzKYu77472502;     nYIRikbHqnCzKYu77472502 = nYIRikbHqnCzKYu35807235;     nYIRikbHqnCzKYu35807235 = nYIRikbHqnCzKYu94801944;     nYIRikbHqnCzKYu94801944 = nYIRikbHqnCzKYu25499313;     nYIRikbHqnCzKYu25499313 = nYIRikbHqnCzKYu28037188;     nYIRikbHqnCzKYu28037188 = nYIRikbHqnCzKYu11964178;     nYIRikbHqnCzKYu11964178 = nYIRikbHqnCzKYu55296038;     nYIRikbHqnCzKYu55296038 = nYIRikbHqnCzKYu56704320;     nYIRikbHqnCzKYu56704320 = nYIRikbHqnCzKYu20536740;     nYIRikbHqnCzKYu20536740 = nYIRikbHqnCzKYu3806084;     nYIRikbHqnCzKYu3806084 = nYIRikbHqnCzKYu95764517;     nYIRikbHqnCzKYu95764517 = nYIRikbHqnCzKYu62095967;     nYIRikbHqnCzKYu62095967 = nYIRikbHqnCzKYu67034041;     nYIRikbHqnCzKYu67034041 = nYIRikbHqnCzKYu47076793;     nYIRikbHqnCzKYu47076793 = nYIRikbHqnCzKYu97299334;     nYIRikbHqnCzKYu97299334 = nYIRikbHqnCzKYu49022430;     nYIRikbHqnCzKYu49022430 = nYIRikbHqnCzKYu84189885;}
// Junk Finished

// Junk Code By Peatreat & Thaisen's Gen
void GKQWkPnSdlmLnQBe73077063() {     int JqRwpLrjNvuFyYe84183413 = -693215746;    int JqRwpLrjNvuFyYe2676812 = -342309207;    int JqRwpLrjNvuFyYe90775153 = -773540913;    int JqRwpLrjNvuFyYe27670824 = -26089830;    int JqRwpLrjNvuFyYe70599330 = -55229408;    int JqRwpLrjNvuFyYe45138856 = -335920770;    int JqRwpLrjNvuFyYe51834817 = -38960317;    int JqRwpLrjNvuFyYe66047246 = -497743420;    int JqRwpLrjNvuFyYe35964035 = -28259078;    int JqRwpLrjNvuFyYe13417411 = -649380348;    int JqRwpLrjNvuFyYe14232491 = -729961051;    int JqRwpLrjNvuFyYe88688514 = -754588429;    int JqRwpLrjNvuFyYe79651993 = -258358804;    int JqRwpLrjNvuFyYe63575141 = -9521576;    int JqRwpLrjNvuFyYe30140959 = -723799062;    int JqRwpLrjNvuFyYe14231984 = -601557755;    int JqRwpLrjNvuFyYe72054944 = 70648616;    int JqRwpLrjNvuFyYe90919012 = -160922422;    int JqRwpLrjNvuFyYe93779777 = -157378410;    int JqRwpLrjNvuFyYe79556657 = -142440564;    int JqRwpLrjNvuFyYe20033306 = -423788951;    int JqRwpLrjNvuFyYe88624621 = -456397165;    int JqRwpLrjNvuFyYe3151836 = -736584909;    int JqRwpLrjNvuFyYe38628619 = -749569133;    int JqRwpLrjNvuFyYe7818357 = -584690981;    int JqRwpLrjNvuFyYe65278878 = -558676906;    int JqRwpLrjNvuFyYe62447709 = -69752221;    int JqRwpLrjNvuFyYe38613044 = 10803747;    int JqRwpLrjNvuFyYe63379284 = -945203359;    int JqRwpLrjNvuFyYe19059070 = -640088393;    int JqRwpLrjNvuFyYe80784805 = -725295403;    int JqRwpLrjNvuFyYe57772215 = -226700675;    int JqRwpLrjNvuFyYe38568609 = -76985207;    int JqRwpLrjNvuFyYe54374462 = -39910391;    int JqRwpLrjNvuFyYe68183745 = -978885540;    int JqRwpLrjNvuFyYe95955284 = -775366766;    int JqRwpLrjNvuFyYe63129118 = -715298723;    int JqRwpLrjNvuFyYe53669716 = -699119277;    int JqRwpLrjNvuFyYe52174107 = -554500579;    int JqRwpLrjNvuFyYe85744705 = -120746203;    int JqRwpLrjNvuFyYe30029597 = -259640464;    int JqRwpLrjNvuFyYe98864061 = -357404591;    int JqRwpLrjNvuFyYe17614824 = -864024807;    int JqRwpLrjNvuFyYe25937003 = -235981902;    int JqRwpLrjNvuFyYe53904011 = -471164469;    int JqRwpLrjNvuFyYe78009382 = -184678176;    int JqRwpLrjNvuFyYe85067603 = -520832776;    int JqRwpLrjNvuFyYe15978585 = -518488691;    int JqRwpLrjNvuFyYe22722963 = -760810729;    int JqRwpLrjNvuFyYe36991101 = -264853095;    int JqRwpLrjNvuFyYe78756067 = -785762561;    int JqRwpLrjNvuFyYe26112149 = -780920494;    int JqRwpLrjNvuFyYe53694761 = -734813146;    int JqRwpLrjNvuFyYe37867144 = -831211066;    int JqRwpLrjNvuFyYe53043758 = -504329971;    int JqRwpLrjNvuFyYe95558792 = -136818582;    int JqRwpLrjNvuFyYe99524975 = -605724299;    int JqRwpLrjNvuFyYe52146535 = 76028219;    int JqRwpLrjNvuFyYe19852468 = -441398850;    int JqRwpLrjNvuFyYe5320452 = -496552503;    int JqRwpLrjNvuFyYe82691146 = -166168549;    int JqRwpLrjNvuFyYe13221774 = 50235935;    int JqRwpLrjNvuFyYe2667963 = -552540062;    int JqRwpLrjNvuFyYe16904966 = -388170685;    int JqRwpLrjNvuFyYe32632605 = -924084945;    int JqRwpLrjNvuFyYe56460276 = -403260377;    int JqRwpLrjNvuFyYe50119905 = -577603223;    int JqRwpLrjNvuFyYe25277531 = -118448414;    int JqRwpLrjNvuFyYe95391395 = -30636037;    int JqRwpLrjNvuFyYe34185674 = -948432296;    int JqRwpLrjNvuFyYe51102865 = -886259032;    int JqRwpLrjNvuFyYe18385229 = -230232107;    int JqRwpLrjNvuFyYe38744905 = -606421843;    int JqRwpLrjNvuFyYe8035073 = 63367793;    int JqRwpLrjNvuFyYe49527060 = -882800101;    int JqRwpLrjNvuFyYe21169244 = 33615640;    int JqRwpLrjNvuFyYe71009797 = -592372358;    int JqRwpLrjNvuFyYe77214833 = -400603007;    int JqRwpLrjNvuFyYe84724608 = -178404664;    int JqRwpLrjNvuFyYe29808974 = -300012805;    int JqRwpLrjNvuFyYe80211275 = 62155870;    int JqRwpLrjNvuFyYe46469124 = -551263531;    int JqRwpLrjNvuFyYe15890082 = -228385524;    int JqRwpLrjNvuFyYe26388183 = -580350264;    int JqRwpLrjNvuFyYe40303003 = -854325832;    int JqRwpLrjNvuFyYe54672656 = -944374910;    int JqRwpLrjNvuFyYe4077454 = -491887529;    int JqRwpLrjNvuFyYe701466 = -245774142;    int JqRwpLrjNvuFyYe1330705 = -535580420;    int JqRwpLrjNvuFyYe72624953 = -742066958;    int JqRwpLrjNvuFyYe96430308 = -69642468;    int JqRwpLrjNvuFyYe10982584 = -691326943;    int JqRwpLrjNvuFyYe33817249 = -157720428;    int JqRwpLrjNvuFyYe46853655 = 42051923;    int JqRwpLrjNvuFyYe3053560 = -954577655;    int JqRwpLrjNvuFyYe16807824 = -209876400;    int JqRwpLrjNvuFyYe96196098 = -804864530;    int JqRwpLrjNvuFyYe709859 = -375854122;    int JqRwpLrjNvuFyYe93304397 = -311896957;    int JqRwpLrjNvuFyYe97443734 = -693215746;     JqRwpLrjNvuFyYe84183413 = JqRwpLrjNvuFyYe2676812;     JqRwpLrjNvuFyYe2676812 = JqRwpLrjNvuFyYe90775153;     JqRwpLrjNvuFyYe90775153 = JqRwpLrjNvuFyYe27670824;     JqRwpLrjNvuFyYe27670824 = JqRwpLrjNvuFyYe70599330;     JqRwpLrjNvuFyYe70599330 = JqRwpLrjNvuFyYe45138856;     JqRwpLrjNvuFyYe45138856 = JqRwpLrjNvuFyYe51834817;     JqRwpLrjNvuFyYe51834817 = JqRwpLrjNvuFyYe66047246;     JqRwpLrjNvuFyYe66047246 = JqRwpLrjNvuFyYe35964035;     JqRwpLrjNvuFyYe35964035 = JqRwpLrjNvuFyYe13417411;     JqRwpLrjNvuFyYe13417411 = JqRwpLrjNvuFyYe14232491;     JqRwpLrjNvuFyYe14232491 = JqRwpLrjNvuFyYe88688514;     JqRwpLrjNvuFyYe88688514 = JqRwpLrjNvuFyYe79651993;     JqRwpLrjNvuFyYe79651993 = JqRwpLrjNvuFyYe63575141;     JqRwpLrjNvuFyYe63575141 = JqRwpLrjNvuFyYe30140959;     JqRwpLrjNvuFyYe30140959 = JqRwpLrjNvuFyYe14231984;     JqRwpLrjNvuFyYe14231984 = JqRwpLrjNvuFyYe72054944;     JqRwpLrjNvuFyYe72054944 = JqRwpLrjNvuFyYe90919012;     JqRwpLrjNvuFyYe90919012 = JqRwpLrjNvuFyYe93779777;     JqRwpLrjNvuFyYe93779777 = JqRwpLrjNvuFyYe79556657;     JqRwpLrjNvuFyYe79556657 = JqRwpLrjNvuFyYe20033306;     JqRwpLrjNvuFyYe20033306 = JqRwpLrjNvuFyYe88624621;     JqRwpLrjNvuFyYe88624621 = JqRwpLrjNvuFyYe3151836;     JqRwpLrjNvuFyYe3151836 = JqRwpLrjNvuFyYe38628619;     JqRwpLrjNvuFyYe38628619 = JqRwpLrjNvuFyYe7818357;     JqRwpLrjNvuFyYe7818357 = JqRwpLrjNvuFyYe65278878;     JqRwpLrjNvuFyYe65278878 = JqRwpLrjNvuFyYe62447709;     JqRwpLrjNvuFyYe62447709 = JqRwpLrjNvuFyYe38613044;     JqRwpLrjNvuFyYe38613044 = JqRwpLrjNvuFyYe63379284;     JqRwpLrjNvuFyYe63379284 = JqRwpLrjNvuFyYe19059070;     JqRwpLrjNvuFyYe19059070 = JqRwpLrjNvuFyYe80784805;     JqRwpLrjNvuFyYe80784805 = JqRwpLrjNvuFyYe57772215;     JqRwpLrjNvuFyYe57772215 = JqRwpLrjNvuFyYe38568609;     JqRwpLrjNvuFyYe38568609 = JqRwpLrjNvuFyYe54374462;     JqRwpLrjNvuFyYe54374462 = JqRwpLrjNvuFyYe68183745;     JqRwpLrjNvuFyYe68183745 = JqRwpLrjNvuFyYe95955284;     JqRwpLrjNvuFyYe95955284 = JqRwpLrjNvuFyYe63129118;     JqRwpLrjNvuFyYe63129118 = JqRwpLrjNvuFyYe53669716;     JqRwpLrjNvuFyYe53669716 = JqRwpLrjNvuFyYe52174107;     JqRwpLrjNvuFyYe52174107 = JqRwpLrjNvuFyYe85744705;     JqRwpLrjNvuFyYe85744705 = JqRwpLrjNvuFyYe30029597;     JqRwpLrjNvuFyYe30029597 = JqRwpLrjNvuFyYe98864061;     JqRwpLrjNvuFyYe98864061 = JqRwpLrjNvuFyYe17614824;     JqRwpLrjNvuFyYe17614824 = JqRwpLrjNvuFyYe25937003;     JqRwpLrjNvuFyYe25937003 = JqRwpLrjNvuFyYe53904011;     JqRwpLrjNvuFyYe53904011 = JqRwpLrjNvuFyYe78009382;     JqRwpLrjNvuFyYe78009382 = JqRwpLrjNvuFyYe85067603;     JqRwpLrjNvuFyYe85067603 = JqRwpLrjNvuFyYe15978585;     JqRwpLrjNvuFyYe15978585 = JqRwpLrjNvuFyYe22722963;     JqRwpLrjNvuFyYe22722963 = JqRwpLrjNvuFyYe36991101;     JqRwpLrjNvuFyYe36991101 = JqRwpLrjNvuFyYe78756067;     JqRwpLrjNvuFyYe78756067 = JqRwpLrjNvuFyYe26112149;     JqRwpLrjNvuFyYe26112149 = JqRwpLrjNvuFyYe53694761;     JqRwpLrjNvuFyYe53694761 = JqRwpLrjNvuFyYe37867144;     JqRwpLrjNvuFyYe37867144 = JqRwpLrjNvuFyYe53043758;     JqRwpLrjNvuFyYe53043758 = JqRwpLrjNvuFyYe95558792;     JqRwpLrjNvuFyYe95558792 = JqRwpLrjNvuFyYe99524975;     JqRwpLrjNvuFyYe99524975 = JqRwpLrjNvuFyYe52146535;     JqRwpLrjNvuFyYe52146535 = JqRwpLrjNvuFyYe19852468;     JqRwpLrjNvuFyYe19852468 = JqRwpLrjNvuFyYe5320452;     JqRwpLrjNvuFyYe5320452 = JqRwpLrjNvuFyYe82691146;     JqRwpLrjNvuFyYe82691146 = JqRwpLrjNvuFyYe13221774;     JqRwpLrjNvuFyYe13221774 = JqRwpLrjNvuFyYe2667963;     JqRwpLrjNvuFyYe2667963 = JqRwpLrjNvuFyYe16904966;     JqRwpLrjNvuFyYe16904966 = JqRwpLrjNvuFyYe32632605;     JqRwpLrjNvuFyYe32632605 = JqRwpLrjNvuFyYe56460276;     JqRwpLrjNvuFyYe56460276 = JqRwpLrjNvuFyYe50119905;     JqRwpLrjNvuFyYe50119905 = JqRwpLrjNvuFyYe25277531;     JqRwpLrjNvuFyYe25277531 = JqRwpLrjNvuFyYe95391395;     JqRwpLrjNvuFyYe95391395 = JqRwpLrjNvuFyYe34185674;     JqRwpLrjNvuFyYe34185674 = JqRwpLrjNvuFyYe51102865;     JqRwpLrjNvuFyYe51102865 = JqRwpLrjNvuFyYe18385229;     JqRwpLrjNvuFyYe18385229 = JqRwpLrjNvuFyYe38744905;     JqRwpLrjNvuFyYe38744905 = JqRwpLrjNvuFyYe8035073;     JqRwpLrjNvuFyYe8035073 = JqRwpLrjNvuFyYe49527060;     JqRwpLrjNvuFyYe49527060 = JqRwpLrjNvuFyYe21169244;     JqRwpLrjNvuFyYe21169244 = JqRwpLrjNvuFyYe71009797;     JqRwpLrjNvuFyYe71009797 = JqRwpLrjNvuFyYe77214833;     JqRwpLrjNvuFyYe77214833 = JqRwpLrjNvuFyYe84724608;     JqRwpLrjNvuFyYe84724608 = JqRwpLrjNvuFyYe29808974;     JqRwpLrjNvuFyYe29808974 = JqRwpLrjNvuFyYe80211275;     JqRwpLrjNvuFyYe80211275 = JqRwpLrjNvuFyYe46469124;     JqRwpLrjNvuFyYe46469124 = JqRwpLrjNvuFyYe15890082;     JqRwpLrjNvuFyYe15890082 = JqRwpLrjNvuFyYe26388183;     JqRwpLrjNvuFyYe26388183 = JqRwpLrjNvuFyYe40303003;     JqRwpLrjNvuFyYe40303003 = JqRwpLrjNvuFyYe54672656;     JqRwpLrjNvuFyYe54672656 = JqRwpLrjNvuFyYe4077454;     JqRwpLrjNvuFyYe4077454 = JqRwpLrjNvuFyYe701466;     JqRwpLrjNvuFyYe701466 = JqRwpLrjNvuFyYe1330705;     JqRwpLrjNvuFyYe1330705 = JqRwpLrjNvuFyYe72624953;     JqRwpLrjNvuFyYe72624953 = JqRwpLrjNvuFyYe96430308;     JqRwpLrjNvuFyYe96430308 = JqRwpLrjNvuFyYe10982584;     JqRwpLrjNvuFyYe10982584 = JqRwpLrjNvuFyYe33817249;     JqRwpLrjNvuFyYe33817249 = JqRwpLrjNvuFyYe46853655;     JqRwpLrjNvuFyYe46853655 = JqRwpLrjNvuFyYe3053560;     JqRwpLrjNvuFyYe3053560 = JqRwpLrjNvuFyYe16807824;     JqRwpLrjNvuFyYe16807824 = JqRwpLrjNvuFyYe96196098;     JqRwpLrjNvuFyYe96196098 = JqRwpLrjNvuFyYe709859;     JqRwpLrjNvuFyYe709859 = JqRwpLrjNvuFyYe93304397;     JqRwpLrjNvuFyYe93304397 = JqRwpLrjNvuFyYe97443734;     JqRwpLrjNvuFyYe97443734 = JqRwpLrjNvuFyYe84183413;}
// Junk Finished

// Junk Code By Peatreat & Thaisen's Gen
void bagRyfEJzlUXngvJ27493276() {     int SVOFYpzRbrgTiQD54835081 = -211364877;    int SVOFYpzRbrgTiQD71350329 = -213952507;    int SVOFYpzRbrgTiQD70744073 = -115835373;    int SVOFYpzRbrgTiQD67602110 = -216228219;    int SVOFYpzRbrgTiQD92049651 = -938635755;    int SVOFYpzRbrgTiQD35138618 = -37501921;    int SVOFYpzRbrgTiQD70213713 = -37736027;    int SVOFYpzRbrgTiQD22889627 = -844560510;    int SVOFYpzRbrgTiQD81829089 = -121606406;    int SVOFYpzRbrgTiQD36383872 = -950475698;    int SVOFYpzRbrgTiQD56481652 = -764862981;    int SVOFYpzRbrgTiQD34235085 = 29647778;    int SVOFYpzRbrgTiQD59479380 = -167895863;    int SVOFYpzRbrgTiQD47422174 = -910549786;    int SVOFYpzRbrgTiQD12860979 = -873549065;    int SVOFYpzRbrgTiQD46760624 = -171541342;    int SVOFYpzRbrgTiQD65907771 = -938536514;    int SVOFYpzRbrgTiQD11212507 = -150088618;    int SVOFYpzRbrgTiQD75487559 = -864566352;    int SVOFYpzRbrgTiQD38343367 = -684806869;    int SVOFYpzRbrgTiQD75169060 = -332468091;    int SVOFYpzRbrgTiQD50020386 = -787682470;    int SVOFYpzRbrgTiQD5095645 = -204428053;    int SVOFYpzRbrgTiQD72478156 = -341970039;    int SVOFYpzRbrgTiQD71942745 = -260595668;    int SVOFYpzRbrgTiQD95876459 = -656961351;    int SVOFYpzRbrgTiQD70708495 = -464929996;    int SVOFYpzRbrgTiQD46395269 = -494320021;    int SVOFYpzRbrgTiQD95521771 = -139561576;    int SVOFYpzRbrgTiQD58231166 = -98483464;    int SVOFYpzRbrgTiQD87856802 = -473729088;    int SVOFYpzRbrgTiQD2283676 = -554997837;    int SVOFYpzRbrgTiQD21513970 = -23946816;    int SVOFYpzRbrgTiQD19739438 = -703440075;    int SVOFYpzRbrgTiQD46207982 = -825832319;    int SVOFYpzRbrgTiQD55948544 = -854593314;    int SVOFYpzRbrgTiQD6920605 = -903891931;    int SVOFYpzRbrgTiQD6462205 = -483232385;    int SVOFYpzRbrgTiQD30707396 = -510229216;    int SVOFYpzRbrgTiQD80848004 = -738354566;    int SVOFYpzRbrgTiQD90102880 = -643659342;    int SVOFYpzRbrgTiQD27347053 = -126793661;    int SVOFYpzRbrgTiQD66581784 = -850778859;    int SVOFYpzRbrgTiQD32445303 = -307956874;    int SVOFYpzRbrgTiQD49858069 = -779810864;    int SVOFYpzRbrgTiQD84415217 = 56984669;    int SVOFYpzRbrgTiQD11198807 = -636902576;    int SVOFYpzRbrgTiQD81592795 = -970618986;    int SVOFYpzRbrgTiQD28164415 = -746574698;    int SVOFYpzRbrgTiQD45418587 = -547861974;    int SVOFYpzRbrgTiQD46945392 = -332334682;    int SVOFYpzRbrgTiQD67983654 = -750140216;    int SVOFYpzRbrgTiQD8770419 = -662611188;    int SVOFYpzRbrgTiQD28094449 = -392092562;    int SVOFYpzRbrgTiQD52324216 = -615270886;    int SVOFYpzRbrgTiQD4814695 = -423682407;    int SVOFYpzRbrgTiQD66254685 = 90475546;    int SVOFYpzRbrgTiQD98265917 = -773865334;    int SVOFYpzRbrgTiQD95659365 = -955632551;    int SVOFYpzRbrgTiQD96173192 = -181674405;    int SVOFYpzRbrgTiQD64430123 = -572571926;    int SVOFYpzRbrgTiQD23818444 = -543416007;    int SVOFYpzRbrgTiQD27367856 = -604998935;    int SVOFYpzRbrgTiQD23597924 = 76877057;    int SVOFYpzRbrgTiQD48527069 = -376746610;    int SVOFYpzRbrgTiQD54197976 = -109865144;    int SVOFYpzRbrgTiQD12721115 = -946405407;    int SVOFYpzRbrgTiQD39739943 = -464455788;    int SVOFYpzRbrgTiQD1214193 = 15282532;    int SVOFYpzRbrgTiQD56912434 = 81044249;    int SVOFYpzRbrgTiQD39840019 = -267649412;    int SVOFYpzRbrgTiQD59445567 = -355304130;    int SVOFYpzRbrgTiQD80505110 = -639859403;    int SVOFYpzRbrgTiQD94639555 = -26211787;    int SVOFYpzRbrgTiQD48240486 = 58852473;    int SVOFYpzRbrgTiQD47822007 = -105674431;    int SVOFYpzRbrgTiQD83438602 = -936903612;    int SVOFYpzRbrgTiQD72650341 = -896471180;    int SVOFYpzRbrgTiQD22620087 = -562159176;    int SVOFYpzRbrgTiQD87527527 = -217580338;    int SVOFYpzRbrgTiQD84677652 = 79941225;    int SVOFYpzRbrgTiQD89115699 = -494311010;    int SVOFYpzRbrgTiQD18230855 = -747745323;    int SVOFYpzRbrgTiQD50103185 = -591699602;    int SVOFYpzRbrgTiQD11285774 = -766148782;    int SVOFYpzRbrgTiQD19873149 = -723588873;    int SVOFYpzRbrgTiQD93513257 = -892386650;    int SVOFYpzRbrgTiQD93419521 = -631854254;    int SVOFYpzRbrgTiQD67415221 = 11830810;    int SVOFYpzRbrgTiQD41393287 = -302149913;    int SVOFYpzRbrgTiQD89693858 = -845068861;    int SVOFYpzRbrgTiQD8654688 = -30026597;    int SVOFYpzRbrgTiQD10802839 = -527599834;    int SVOFYpzRbrgTiQD34534204 = -228554811;    int SVOFYpzRbrgTiQD16417881 = -65782641;    int SVOFYpzRbrgTiQD66284437 = -243336;    int SVOFYpzRbrgTiQD99979197 = -521794726;    int SVOFYpzRbrgTiQD42983860 = -827655916;    int SVOFYpzRbrgTiQD83918233 = -931210264;    int SVOFYpzRbrgTiQD95660093 = -211364877;     SVOFYpzRbrgTiQD54835081 = SVOFYpzRbrgTiQD71350329;     SVOFYpzRbrgTiQD71350329 = SVOFYpzRbrgTiQD70744073;     SVOFYpzRbrgTiQD70744073 = SVOFYpzRbrgTiQD67602110;     SVOFYpzRbrgTiQD67602110 = SVOFYpzRbrgTiQD92049651;     SVOFYpzRbrgTiQD92049651 = SVOFYpzRbrgTiQD35138618;     SVOFYpzRbrgTiQD35138618 = SVOFYpzRbrgTiQD70213713;     SVOFYpzRbrgTiQD70213713 = SVOFYpzRbrgTiQD22889627;     SVOFYpzRbrgTiQD22889627 = SVOFYpzRbrgTiQD81829089;     SVOFYpzRbrgTiQD81829089 = SVOFYpzRbrgTiQD36383872;     SVOFYpzRbrgTiQD36383872 = SVOFYpzRbrgTiQD56481652;     SVOFYpzRbrgTiQD56481652 = SVOFYpzRbrgTiQD34235085;     SVOFYpzRbrgTiQD34235085 = SVOFYpzRbrgTiQD59479380;     SVOFYpzRbrgTiQD59479380 = SVOFYpzRbrgTiQD47422174;     SVOFYpzRbrgTiQD47422174 = SVOFYpzRbrgTiQD12860979;     SVOFYpzRbrgTiQD12860979 = SVOFYpzRbrgTiQD46760624;     SVOFYpzRbrgTiQD46760624 = SVOFYpzRbrgTiQD65907771;     SVOFYpzRbrgTiQD65907771 = SVOFYpzRbrgTiQD11212507;     SVOFYpzRbrgTiQD11212507 = SVOFYpzRbrgTiQD75487559;     SVOFYpzRbrgTiQD75487559 = SVOFYpzRbrgTiQD38343367;     SVOFYpzRbrgTiQD38343367 = SVOFYpzRbrgTiQD75169060;     SVOFYpzRbrgTiQD75169060 = SVOFYpzRbrgTiQD50020386;     SVOFYpzRbrgTiQD50020386 = SVOFYpzRbrgTiQD5095645;     SVOFYpzRbrgTiQD5095645 = SVOFYpzRbrgTiQD72478156;     SVOFYpzRbrgTiQD72478156 = SVOFYpzRbrgTiQD71942745;     SVOFYpzRbrgTiQD71942745 = SVOFYpzRbrgTiQD95876459;     SVOFYpzRbrgTiQD95876459 = SVOFYpzRbrgTiQD70708495;     SVOFYpzRbrgTiQD70708495 = SVOFYpzRbrgTiQD46395269;     SVOFYpzRbrgTiQD46395269 = SVOFYpzRbrgTiQD95521771;     SVOFYpzRbrgTiQD95521771 = SVOFYpzRbrgTiQD58231166;     SVOFYpzRbrgTiQD58231166 = SVOFYpzRbrgTiQD87856802;     SVOFYpzRbrgTiQD87856802 = SVOFYpzRbrgTiQD2283676;     SVOFYpzRbrgTiQD2283676 = SVOFYpzRbrgTiQD21513970;     SVOFYpzRbrgTiQD21513970 = SVOFYpzRbrgTiQD19739438;     SVOFYpzRbrgTiQD19739438 = SVOFYpzRbrgTiQD46207982;     SVOFYpzRbrgTiQD46207982 = SVOFYpzRbrgTiQD55948544;     SVOFYpzRbrgTiQD55948544 = SVOFYpzRbrgTiQD6920605;     SVOFYpzRbrgTiQD6920605 = SVOFYpzRbrgTiQD6462205;     SVOFYpzRbrgTiQD6462205 = SVOFYpzRbrgTiQD30707396;     SVOFYpzRbrgTiQD30707396 = SVOFYpzRbrgTiQD80848004;     SVOFYpzRbrgTiQD80848004 = SVOFYpzRbrgTiQD90102880;     SVOFYpzRbrgTiQD90102880 = SVOFYpzRbrgTiQD27347053;     SVOFYpzRbrgTiQD27347053 = SVOFYpzRbrgTiQD66581784;     SVOFYpzRbrgTiQD66581784 = SVOFYpzRbrgTiQD32445303;     SVOFYpzRbrgTiQD32445303 = SVOFYpzRbrgTiQD49858069;     SVOFYpzRbrgTiQD49858069 = SVOFYpzRbrgTiQD84415217;     SVOFYpzRbrgTiQD84415217 = SVOFYpzRbrgTiQD11198807;     SVOFYpzRbrgTiQD11198807 = SVOFYpzRbrgTiQD81592795;     SVOFYpzRbrgTiQD81592795 = SVOFYpzRbrgTiQD28164415;     SVOFYpzRbrgTiQD28164415 = SVOFYpzRbrgTiQD45418587;     SVOFYpzRbrgTiQD45418587 = SVOFYpzRbrgTiQD46945392;     SVOFYpzRbrgTiQD46945392 = SVOFYpzRbrgTiQD67983654;     SVOFYpzRbrgTiQD67983654 = SVOFYpzRbrgTiQD8770419;     SVOFYpzRbrgTiQD8770419 = SVOFYpzRbrgTiQD28094449;     SVOFYpzRbrgTiQD28094449 = SVOFYpzRbrgTiQD52324216;     SVOFYpzRbrgTiQD52324216 = SVOFYpzRbrgTiQD4814695;     SVOFYpzRbrgTiQD4814695 = SVOFYpzRbrgTiQD66254685;     SVOFYpzRbrgTiQD66254685 = SVOFYpzRbrgTiQD98265917;     SVOFYpzRbrgTiQD98265917 = SVOFYpzRbrgTiQD95659365;     SVOFYpzRbrgTiQD95659365 = SVOFYpzRbrgTiQD96173192;     SVOFYpzRbrgTiQD96173192 = SVOFYpzRbrgTiQD64430123;     SVOFYpzRbrgTiQD64430123 = SVOFYpzRbrgTiQD23818444;     SVOFYpzRbrgTiQD23818444 = SVOFYpzRbrgTiQD27367856;     SVOFYpzRbrgTiQD27367856 = SVOFYpzRbrgTiQD23597924;     SVOFYpzRbrgTiQD23597924 = SVOFYpzRbrgTiQD48527069;     SVOFYpzRbrgTiQD48527069 = SVOFYpzRbrgTiQD54197976;     SVOFYpzRbrgTiQD54197976 = SVOFYpzRbrgTiQD12721115;     SVOFYpzRbrgTiQD12721115 = SVOFYpzRbrgTiQD39739943;     SVOFYpzRbrgTiQD39739943 = SVOFYpzRbrgTiQD1214193;     SVOFYpzRbrgTiQD1214193 = SVOFYpzRbrgTiQD56912434;     SVOFYpzRbrgTiQD56912434 = SVOFYpzRbrgTiQD39840019;     SVOFYpzRbrgTiQD39840019 = SVOFYpzRbrgTiQD59445567;     SVOFYpzRbrgTiQD59445567 = SVOFYpzRbrgTiQD80505110;     SVOFYpzRbrgTiQD80505110 = SVOFYpzRbrgTiQD94639555;     SVOFYpzRbrgTiQD94639555 = SVOFYpzRbrgTiQD48240486;     SVOFYpzRbrgTiQD48240486 = SVOFYpzRbrgTiQD47822007;     SVOFYpzRbrgTiQD47822007 = SVOFYpzRbrgTiQD83438602;     SVOFYpzRbrgTiQD83438602 = SVOFYpzRbrgTiQD72650341;     SVOFYpzRbrgTiQD72650341 = SVOFYpzRbrgTiQD22620087;     SVOFYpzRbrgTiQD22620087 = SVOFYpzRbrgTiQD87527527;     SVOFYpzRbrgTiQD87527527 = SVOFYpzRbrgTiQD84677652;     SVOFYpzRbrgTiQD84677652 = SVOFYpzRbrgTiQD89115699;     SVOFYpzRbrgTiQD89115699 = SVOFYpzRbrgTiQD18230855;     SVOFYpzRbrgTiQD18230855 = SVOFYpzRbrgTiQD50103185;     SVOFYpzRbrgTiQD50103185 = SVOFYpzRbrgTiQD11285774;     SVOFYpzRbrgTiQD11285774 = SVOFYpzRbrgTiQD19873149;     SVOFYpzRbrgTiQD19873149 = SVOFYpzRbrgTiQD93513257;     SVOFYpzRbrgTiQD93513257 = SVOFYpzRbrgTiQD93419521;     SVOFYpzRbrgTiQD93419521 = SVOFYpzRbrgTiQD67415221;     SVOFYpzRbrgTiQD67415221 = SVOFYpzRbrgTiQD41393287;     SVOFYpzRbrgTiQD41393287 = SVOFYpzRbrgTiQD89693858;     SVOFYpzRbrgTiQD89693858 = SVOFYpzRbrgTiQD8654688;     SVOFYpzRbrgTiQD8654688 = SVOFYpzRbrgTiQD10802839;     SVOFYpzRbrgTiQD10802839 = SVOFYpzRbrgTiQD34534204;     SVOFYpzRbrgTiQD34534204 = SVOFYpzRbrgTiQD16417881;     SVOFYpzRbrgTiQD16417881 = SVOFYpzRbrgTiQD66284437;     SVOFYpzRbrgTiQD66284437 = SVOFYpzRbrgTiQD99979197;     SVOFYpzRbrgTiQD99979197 = SVOFYpzRbrgTiQD42983860;     SVOFYpzRbrgTiQD42983860 = SVOFYpzRbrgTiQD83918233;     SVOFYpzRbrgTiQD83918233 = SVOFYpzRbrgTiQD95660093;     SVOFYpzRbrgTiQD95660093 = SVOFYpzRbrgTiQD54835081;}
// Junk Finished

// Junk Code By Peatreat & Thaisen's Gen
void rhlMoDyhnobyqBgx34152021() {     int QUKHPjxaOhDhMzi54828609 = -83889018;    int QUKHPjxaOhDhMzi9922646 = -256547632;    int QUKHPjxaOhDhMzi72057285 = -72963986;    int QUKHPjxaOhDhMzi52681971 = -36769049;    int QUKHPjxaOhDhMzi889754 = -306991077;    int QUKHPjxaOhDhMzi69944050 = -836934733;    int QUKHPjxaOhDhMzi38761049 = -842272494;    int QUKHPjxaOhDhMzi92103677 = -548737827;    int QUKHPjxaOhDhMzi64569254 = -935252476;    int QUKHPjxaOhDhMzi33023385 = -587282570;    int QUKHPjxaOhDhMzi76802257 = -724335053;    int QUKHPjxaOhDhMzi48526762 = -805830585;    int QUKHPjxaOhDhMzi91332294 = -189554337;    int QUKHPjxaOhDhMzi1266395 = -155702838;    int QUKHPjxaOhDhMzi58033087 = -836261917;    int QUKHPjxaOhDhMzi21325171 = -504336992;    int QUKHPjxaOhDhMzi21760531 = -758151084;    int QUKHPjxaOhDhMzi34213039 = -547160240;    int QUKHPjxaOhDhMzi95915829 = -382548619;    int QUKHPjxaOhDhMzi10417130 = -196831320;    int QUKHPjxaOhDhMzi54838484 = -42934465;    int QUKHPjxaOhDhMzi28316652 = -197674354;    int QUKHPjxaOhDhMzi41974041 = -663385895;    int QUKHPjxaOhDhMzi16128264 = -222890562;    int QUKHPjxaOhDhMzi385435 = -687289521;    int QUKHPjxaOhDhMzi12450536 = -565167279;    int QUKHPjxaOhDhMzi40930077 = -765729709;    int QUKHPjxaOhDhMzi81798794 = -3287009;    int QUKHPjxaOhDhMzi8200219 = -785631890;    int QUKHPjxaOhDhMzi34321036 = -873914837;    int QUKHPjxaOhDhMzi21269481 = -860822323;    int QUKHPjxaOhDhMzi87007533 = -946819599;    int QUKHPjxaOhDhMzi6966394 = -225891032;    int QUKHPjxaOhDhMzi20022275 = -580121807;    int QUKHPjxaOhDhMzi28566020 = 14853756;    int QUKHPjxaOhDhMzi98776733 = -934359061;    int QUKHPjxaOhDhMzi18861973 = 96454691;    int QUKHPjxaOhDhMzi76379884 = -478495064;    int QUKHPjxaOhDhMzi66020992 = -122910632;    int QUKHPjxaOhDhMzi52720897 = -347786252;    int QUKHPjxaOhDhMzi77958548 = -731603241;    int QUKHPjxaOhDhMzi13044714 = -320585743;    int QUKHPjxaOhDhMzi26865144 = -309909113;    int QUKHPjxaOhDhMzi91677201 = -964921317;    int QUKHPjxaOhDhMzi33876122 = -153070265;    int QUKHPjxaOhDhMzi24843186 = -855060002;    int QUKHPjxaOhDhMzi38896599 = -18183198;    int QUKHPjxaOhDhMzi56976376 = -479380342;    int QUKHPjxaOhDhMzi46822804 = -310338560;    int QUKHPjxaOhDhMzi65464010 = -134886860;    int QUKHPjxaOhDhMzi60204762 = 89110310;    int QUKHPjxaOhDhMzi82530911 = -32285964;    int QUKHPjxaOhDhMzi84218766 = -554665196;    int QUKHPjxaOhDhMzi38344720 = -924012414;    int QUKHPjxaOhDhMzi79313537 = -372170163;    int QUKHPjxaOhDhMzi26511958 = -886214665;    int QUKHPjxaOhDhMzi67948605 = -593161737;    int QUKHPjxaOhDhMzi55929021 = -850073425;    int QUKHPjxaOhDhMzi52296537 = -349479528;    int QUKHPjxaOhDhMzi88439217 = -741823798;    int QUKHPjxaOhDhMzi29013974 = 28794975;    int QUKHPjxaOhDhMzi56962255 = -738985486;    int QUKHPjxaOhDhMzi83903459 = -763105937;    int QUKHPjxaOhDhMzi30248218 = 38662360;    int QUKHPjxaOhDhMzi11753905 = -726460247;    int QUKHPjxaOhDhMzi89794724 = -777515454;    int QUKHPjxaOhDhMzi41560369 = -479939553;    int QUKHPjxaOhDhMzi71310020 = -609432530;    int QUKHPjxaOhDhMzi72700374 = -70556594;    int QUKHPjxaOhDhMzi59256354 = -901902857;    int QUKHPjxaOhDhMzi2463199 = -500791684;    int QUKHPjxaOhDhMzi45380647 = -179656020;    int QUKHPjxaOhDhMzi68192047 = -324249608;    int QUKHPjxaOhDhMzi43194933 = 65237633;    int QUKHPjxaOhDhMzi32458581 = -465228080;    int QUKHPjxaOhDhMzi41793770 = -722348723;    int QUKHPjxaOhDhMzi1451508 = -887765241;    int QUKHPjxaOhDhMzi50296840 = -698464578;    int QUKHPjxaOhDhMzi82252142 = 30179703;    int QUKHPjxaOhDhMzi75542249 = -832229520;    int QUKHPjxaOhDhMzi73553937 = -446984082;    int QUKHPjxaOhDhMzi83953700 = -186349367;    int QUKHPjxaOhDhMzi34975990 = -692948450;    int QUKHPjxaOhDhMzi42736208 = -550745031;    int QUKHPjxaOhDhMzi74116274 = -863025148;    int QUKHPjxaOhDhMzi38738570 = -728536360;    int QUKHPjxaOhDhMzi2788767 = -292154403;    int QUKHPjxaOhDhMzi68621674 = -301878619;    int QUKHPjxaOhDhMzi40708738 = -107951645;    int QUKHPjxaOhDhMzi2054063 = -98931580;    int QUKHPjxaOhDhMzi30828129 = -241197324;    int QUKHPjxaOhDhMzi62932951 = -53471884;    int QUKHPjxaOhDhMzi24083348 = -29015536;    int QUKHPjxaOhDhMzi77581775 = -381086834;    int QUKHPjxaOhDhMzi23706923 = -276581228;    int QUKHPjxaOhDhMzi20996294 = -992617755;    int QUKHPjxaOhDhMzi29141254 = -557479806;    int QUKHPjxaOhDhMzi96616925 = -248571474;    int QUKHPjxaOhDhMzi79923297 = -138461071;    int QUKHPjxaOhDhMzi44081398 = -83889018;     QUKHPjxaOhDhMzi54828609 = QUKHPjxaOhDhMzi9922646;     QUKHPjxaOhDhMzi9922646 = QUKHPjxaOhDhMzi72057285;     QUKHPjxaOhDhMzi72057285 = QUKHPjxaOhDhMzi52681971;     QUKHPjxaOhDhMzi52681971 = QUKHPjxaOhDhMzi889754;     QUKHPjxaOhDhMzi889754 = QUKHPjxaOhDhMzi69944050;     QUKHPjxaOhDhMzi69944050 = QUKHPjxaOhDhMzi38761049;     QUKHPjxaOhDhMzi38761049 = QUKHPjxaOhDhMzi92103677;     QUKHPjxaOhDhMzi92103677 = QUKHPjxaOhDhMzi64569254;     QUKHPjxaOhDhMzi64569254 = QUKHPjxaOhDhMzi33023385;     QUKHPjxaOhDhMzi33023385 = QUKHPjxaOhDhMzi76802257;     QUKHPjxaOhDhMzi76802257 = QUKHPjxaOhDhMzi48526762;     QUKHPjxaOhDhMzi48526762 = QUKHPjxaOhDhMzi91332294;     QUKHPjxaOhDhMzi91332294 = QUKHPjxaOhDhMzi1266395;     QUKHPjxaOhDhMzi1266395 = QUKHPjxaOhDhMzi58033087;     QUKHPjxaOhDhMzi58033087 = QUKHPjxaOhDhMzi21325171;     QUKHPjxaOhDhMzi21325171 = QUKHPjxaOhDhMzi21760531;     QUKHPjxaOhDhMzi21760531 = QUKHPjxaOhDhMzi34213039;     QUKHPjxaOhDhMzi34213039 = QUKHPjxaOhDhMzi95915829;     QUKHPjxaOhDhMzi95915829 = QUKHPjxaOhDhMzi10417130;     QUKHPjxaOhDhMzi10417130 = QUKHPjxaOhDhMzi54838484;     QUKHPjxaOhDhMzi54838484 = QUKHPjxaOhDhMzi28316652;     QUKHPjxaOhDhMzi28316652 = QUKHPjxaOhDhMzi41974041;     QUKHPjxaOhDhMzi41974041 = QUKHPjxaOhDhMzi16128264;     QUKHPjxaOhDhMzi16128264 = QUKHPjxaOhDhMzi385435;     QUKHPjxaOhDhMzi385435 = QUKHPjxaOhDhMzi12450536;     QUKHPjxaOhDhMzi12450536 = QUKHPjxaOhDhMzi40930077;     QUKHPjxaOhDhMzi40930077 = QUKHPjxaOhDhMzi81798794;     QUKHPjxaOhDhMzi81798794 = QUKHPjxaOhDhMzi8200219;     QUKHPjxaOhDhMzi8200219 = QUKHPjxaOhDhMzi34321036;     QUKHPjxaOhDhMzi34321036 = QUKHPjxaOhDhMzi21269481;     QUKHPjxaOhDhMzi21269481 = QUKHPjxaOhDhMzi87007533;     QUKHPjxaOhDhMzi87007533 = QUKHPjxaOhDhMzi6966394;     QUKHPjxaOhDhMzi6966394 = QUKHPjxaOhDhMzi20022275;     QUKHPjxaOhDhMzi20022275 = QUKHPjxaOhDhMzi28566020;     QUKHPjxaOhDhMzi28566020 = QUKHPjxaOhDhMzi98776733;     QUKHPjxaOhDhMzi98776733 = QUKHPjxaOhDhMzi18861973;     QUKHPjxaOhDhMzi18861973 = QUKHPjxaOhDhMzi76379884;     QUKHPjxaOhDhMzi76379884 = QUKHPjxaOhDhMzi66020992;     QUKHPjxaOhDhMzi66020992 = QUKHPjxaOhDhMzi52720897;     QUKHPjxaOhDhMzi52720897 = QUKHPjxaOhDhMzi77958548;     QUKHPjxaOhDhMzi77958548 = QUKHPjxaOhDhMzi13044714;     QUKHPjxaOhDhMzi13044714 = QUKHPjxaOhDhMzi26865144;     QUKHPjxaOhDhMzi26865144 = QUKHPjxaOhDhMzi91677201;     QUKHPjxaOhDhMzi91677201 = QUKHPjxaOhDhMzi33876122;     QUKHPjxaOhDhMzi33876122 = QUKHPjxaOhDhMzi24843186;     QUKHPjxaOhDhMzi24843186 = QUKHPjxaOhDhMzi38896599;     QUKHPjxaOhDhMzi38896599 = QUKHPjxaOhDhMzi56976376;     QUKHPjxaOhDhMzi56976376 = QUKHPjxaOhDhMzi46822804;     QUKHPjxaOhDhMzi46822804 = QUKHPjxaOhDhMzi65464010;     QUKHPjxaOhDhMzi65464010 = QUKHPjxaOhDhMzi60204762;     QUKHPjxaOhDhMzi60204762 = QUKHPjxaOhDhMzi82530911;     QUKHPjxaOhDhMzi82530911 = QUKHPjxaOhDhMzi84218766;     QUKHPjxaOhDhMzi84218766 = QUKHPjxaOhDhMzi38344720;     QUKHPjxaOhDhMzi38344720 = QUKHPjxaOhDhMzi79313537;     QUKHPjxaOhDhMzi79313537 = QUKHPjxaOhDhMzi26511958;     QUKHPjxaOhDhMzi26511958 = QUKHPjxaOhDhMzi67948605;     QUKHPjxaOhDhMzi67948605 = QUKHPjxaOhDhMzi55929021;     QUKHPjxaOhDhMzi55929021 = QUKHPjxaOhDhMzi52296537;     QUKHPjxaOhDhMzi52296537 = QUKHPjxaOhDhMzi88439217;     QUKHPjxaOhDhMzi88439217 = QUKHPjxaOhDhMzi29013974;     QUKHPjxaOhDhMzi29013974 = QUKHPjxaOhDhMzi56962255;     QUKHPjxaOhDhMzi56962255 = QUKHPjxaOhDhMzi83903459;     QUKHPjxaOhDhMzi83903459 = QUKHPjxaOhDhMzi30248218;     QUKHPjxaOhDhMzi30248218 = QUKHPjxaOhDhMzi11753905;     QUKHPjxaOhDhMzi11753905 = QUKHPjxaOhDhMzi89794724;     QUKHPjxaOhDhMzi89794724 = QUKHPjxaOhDhMzi41560369;     QUKHPjxaOhDhMzi41560369 = QUKHPjxaOhDhMzi71310020;     QUKHPjxaOhDhMzi71310020 = QUKHPjxaOhDhMzi72700374;     QUKHPjxaOhDhMzi72700374 = QUKHPjxaOhDhMzi59256354;     QUKHPjxaOhDhMzi59256354 = QUKHPjxaOhDhMzi2463199;     QUKHPjxaOhDhMzi2463199 = QUKHPjxaOhDhMzi45380647;     QUKHPjxaOhDhMzi45380647 = QUKHPjxaOhDhMzi68192047;     QUKHPjxaOhDhMzi68192047 = QUKHPjxaOhDhMzi43194933;     QUKHPjxaOhDhMzi43194933 = QUKHPjxaOhDhMzi32458581;     QUKHPjxaOhDhMzi32458581 = QUKHPjxaOhDhMzi41793770;     QUKHPjxaOhDhMzi41793770 = QUKHPjxaOhDhMzi1451508;     QUKHPjxaOhDhMzi1451508 = QUKHPjxaOhDhMzi50296840;     QUKHPjxaOhDhMzi50296840 = QUKHPjxaOhDhMzi82252142;     QUKHPjxaOhDhMzi82252142 = QUKHPjxaOhDhMzi75542249;     QUKHPjxaOhDhMzi75542249 = QUKHPjxaOhDhMzi73553937;     QUKHPjxaOhDhMzi73553937 = QUKHPjxaOhDhMzi83953700;     QUKHPjxaOhDhMzi83953700 = QUKHPjxaOhDhMzi34975990;     QUKHPjxaOhDhMzi34975990 = QUKHPjxaOhDhMzi42736208;     QUKHPjxaOhDhMzi42736208 = QUKHPjxaOhDhMzi74116274;     QUKHPjxaOhDhMzi74116274 = QUKHPjxaOhDhMzi38738570;     QUKHPjxaOhDhMzi38738570 = QUKHPjxaOhDhMzi2788767;     QUKHPjxaOhDhMzi2788767 = QUKHPjxaOhDhMzi68621674;     QUKHPjxaOhDhMzi68621674 = QUKHPjxaOhDhMzi40708738;     QUKHPjxaOhDhMzi40708738 = QUKHPjxaOhDhMzi2054063;     QUKHPjxaOhDhMzi2054063 = QUKHPjxaOhDhMzi30828129;     QUKHPjxaOhDhMzi30828129 = QUKHPjxaOhDhMzi62932951;     QUKHPjxaOhDhMzi62932951 = QUKHPjxaOhDhMzi24083348;     QUKHPjxaOhDhMzi24083348 = QUKHPjxaOhDhMzi77581775;     QUKHPjxaOhDhMzi77581775 = QUKHPjxaOhDhMzi23706923;     QUKHPjxaOhDhMzi23706923 = QUKHPjxaOhDhMzi20996294;     QUKHPjxaOhDhMzi20996294 = QUKHPjxaOhDhMzi29141254;     QUKHPjxaOhDhMzi29141254 = QUKHPjxaOhDhMzi96616925;     QUKHPjxaOhDhMzi96616925 = QUKHPjxaOhDhMzi79923297;     QUKHPjxaOhDhMzi79923297 = QUKHPjxaOhDhMzi44081398;     QUKHPjxaOhDhMzi44081398 = QUKHPjxaOhDhMzi54828609;}
// Junk Finished

// Junk Code By Peatreat & Thaisen's Gen
void AsxiugMBQHsqXuml88568234() {     int EeLOOcbIlOvbyGU25480277 = -702038148;    int EeLOOcbIlOvbyGU78596163 = -128190931;    int EeLOOcbIlOvbyGU52026204 = -515258446;    int EeLOOcbIlOvbyGU92613257 = -226907438;    int EeLOOcbIlOvbyGU22340075 = -90397424;    int EeLOOcbIlOvbyGU59943813 = -538515885;    int EeLOOcbIlOvbyGU57139944 = -841048204;    int EeLOOcbIlOvbyGU48946058 = -895554917;    int EeLOOcbIlOvbyGU10434309 = 71400195;    int EeLOOcbIlOvbyGU55989846 = -888377920;    int EeLOOcbIlOvbyGU19051418 = -759236982;    int EeLOOcbIlOvbyGU94073333 = -21594378;    int EeLOOcbIlOvbyGU71159681 = -99091396;    int EeLOOcbIlOvbyGU85113428 = 43268952;    int EeLOOcbIlOvbyGU40753107 = -986011920;    int EeLOOcbIlOvbyGU53853811 = -74320579;    int EeLOOcbIlOvbyGU15613358 = -667336214;    int EeLOOcbIlOvbyGU54506534 = -536326436;    int EeLOOcbIlOvbyGU77623611 = 10263439;    int EeLOOcbIlOvbyGU69203838 = -739197625;    int EeLOOcbIlOvbyGU9974239 = 48386395;    int EeLOOcbIlOvbyGU89712416 = -528959659;    int EeLOOcbIlOvbyGU43917850 = -131229040;    int EeLOOcbIlOvbyGU49977801 = -915291468;    int EeLOOcbIlOvbyGU64509823 = -363194209;    int EeLOOcbIlOvbyGU43048117 = -663451724;    int EeLOOcbIlOvbyGU49190862 = -60907484;    int EeLOOcbIlOvbyGU89581019 = -508410776;    int EeLOOcbIlOvbyGU40342706 = 20009893;    int EeLOOcbIlOvbyGU73493132 = -332309908;    int EeLOOcbIlOvbyGU28341478 = -609256008;    int EeLOOcbIlOvbyGU31518994 = -175116762;    int EeLOOcbIlOvbyGU89911754 = -172852641;    int EeLOOcbIlOvbyGU85387249 = -143651492;    int EeLOOcbIlOvbyGU6590257 = -932093023;    int EeLOOcbIlOvbyGU58769992 = 86414392;    int EeLOOcbIlOvbyGU62653458 = -92138516;    int EeLOOcbIlOvbyGU29172373 = -262608171;    int EeLOOcbIlOvbyGU44554281 = -78639269;    int EeLOOcbIlOvbyGU47824195 = -965394615;    int EeLOOcbIlOvbyGU38031832 = -15622119;    int EeLOOcbIlOvbyGU41527705 = -89974812;    int EeLOOcbIlOvbyGU75832104 = -296663165;    int EeLOOcbIlOvbyGU98185501 = 63103711;    int EeLOOcbIlOvbyGU29830181 = -461716660;    int EeLOOcbIlOvbyGU31249021 = -613397157;    int EeLOOcbIlOvbyGU65027802 = -134252997;    int EeLOOcbIlOvbyGU22590586 = -931510638;    int EeLOOcbIlOvbyGU52264256 = -296102529;    int EeLOOcbIlOvbyGU73891496 = -417895739;    int EeLOOcbIlOvbyGU28394086 = -557461811;    int EeLOOcbIlOvbyGU24402416 = -1505686;    int EeLOOcbIlOvbyGU39294424 = -482463238;    int EeLOOcbIlOvbyGU28572024 = -484893910;    int EeLOOcbIlOvbyGU78593995 = -483111078;    int EeLOOcbIlOvbyGU35767860 = -73078490;    int EeLOOcbIlOvbyGU34678314 = -996961892;    int EeLOOcbIlOvbyGU2048403 = -599966979;    int EeLOOcbIlOvbyGU28103435 = -863713230;    int EeLOOcbIlOvbyGU79291958 = -426945701;    int EeLOOcbIlOvbyGU10752951 = -377608401;    int EeLOOcbIlOvbyGU67558925 = -232637428;    int EeLOOcbIlOvbyGU8603352 = -815564810;    int EeLOOcbIlOvbyGU36941177 = -596289898;    int EeLOOcbIlOvbyGU27648369 = -179121912;    int EeLOOcbIlOvbyGU87532424 = -484120221;    int EeLOOcbIlOvbyGU4161580 = -848741737;    int EeLOOcbIlOvbyGU85772432 = -955439904;    int EeLOOcbIlOvbyGU78523172 = -24638025;    int EeLOOcbIlOvbyGU81983114 = -972426312;    int EeLOOcbIlOvbyGU91200353 = -982182063;    int EeLOOcbIlOvbyGU86440984 = -304728043;    int EeLOOcbIlOvbyGU9952253 = -357687168;    int EeLOOcbIlOvbyGU29799416 = -24341947;    int EeLOOcbIlOvbyGU31172007 = -623575506;    int EeLOOcbIlOvbyGU68446534 = -861638794;    int EeLOOcbIlOvbyGU13880313 = -132296494;    int EeLOOcbIlOvbyGU45732348 = -94332752;    int EeLOOcbIlOvbyGU20147621 = -353574808;    int EeLOOcbIlOvbyGU33260803 = -749797053;    int EeLOOcbIlOvbyGU78020314 = -429198727;    int EeLOOcbIlOvbyGU26600276 = -129396847;    int EeLOOcbIlOvbyGU37316763 = -112308248;    int EeLOOcbIlOvbyGU66451210 = -562094369;    int EeLOOcbIlOvbyGU45099046 = -774848098;    int EeLOOcbIlOvbyGU3939062 = -507750323;    int EeLOOcbIlOvbyGU92224570 = -692653524;    int EeLOOcbIlOvbyGU61339730 = -687958731;    int EeLOOcbIlOvbyGU6793254 = -660540415;    int EeLOOcbIlOvbyGU70822396 = -759014534;    int EeLOOcbIlOvbyGU24091679 = 83376283;    int EeLOOcbIlOvbyGU60605055 = -492171538;    int EeLOOcbIlOvbyGU1068938 = -398894942;    int EeLOOcbIlOvbyGU65262323 = -651693568;    int EeLOOcbIlOvbyGU37071244 = -487786215;    int EeLOOcbIlOvbyGU70472907 = -782984691;    int EeLOOcbIlOvbyGU32924353 = -274410003;    int EeLOOcbIlOvbyGU38890928 = -700373268;    int EeLOOcbIlOvbyGU70537133 = -757774377;    int EeLOOcbIlOvbyGU42297756 = -702038148;     EeLOOcbIlOvbyGU25480277 = EeLOOcbIlOvbyGU78596163;     EeLOOcbIlOvbyGU78596163 = EeLOOcbIlOvbyGU52026204;     EeLOOcbIlOvbyGU52026204 = EeLOOcbIlOvbyGU92613257;     EeLOOcbIlOvbyGU92613257 = EeLOOcbIlOvbyGU22340075;     EeLOOcbIlOvbyGU22340075 = EeLOOcbIlOvbyGU59943813;     EeLOOcbIlOvbyGU59943813 = EeLOOcbIlOvbyGU57139944;     EeLOOcbIlOvbyGU57139944 = EeLOOcbIlOvbyGU48946058;     EeLOOcbIlOvbyGU48946058 = EeLOOcbIlOvbyGU10434309;     EeLOOcbIlOvbyGU10434309 = EeLOOcbIlOvbyGU55989846;     EeLOOcbIlOvbyGU55989846 = EeLOOcbIlOvbyGU19051418;     EeLOOcbIlOvbyGU19051418 = EeLOOcbIlOvbyGU94073333;     EeLOOcbIlOvbyGU94073333 = EeLOOcbIlOvbyGU71159681;     EeLOOcbIlOvbyGU71159681 = EeLOOcbIlOvbyGU85113428;     EeLOOcbIlOvbyGU85113428 = EeLOOcbIlOvbyGU40753107;     EeLOOcbIlOvbyGU40753107 = EeLOOcbIlOvbyGU53853811;     EeLOOcbIlOvbyGU53853811 = EeLOOcbIlOvbyGU15613358;     EeLOOcbIlOvbyGU15613358 = EeLOOcbIlOvbyGU54506534;     EeLOOcbIlOvbyGU54506534 = EeLOOcbIlOvbyGU77623611;     EeLOOcbIlOvbyGU77623611 = EeLOOcbIlOvbyGU69203838;     EeLOOcbIlOvbyGU69203838 = EeLOOcbIlOvbyGU9974239;     EeLOOcbIlOvbyGU9974239 = EeLOOcbIlOvbyGU89712416;     EeLOOcbIlOvbyGU89712416 = EeLOOcbIlOvbyGU43917850;     EeLOOcbIlOvbyGU43917850 = EeLOOcbIlOvbyGU49977801;     EeLOOcbIlOvbyGU49977801 = EeLOOcbIlOvbyGU64509823;     EeLOOcbIlOvbyGU64509823 = EeLOOcbIlOvbyGU43048117;     EeLOOcbIlOvbyGU43048117 = EeLOOcbIlOvbyGU49190862;     EeLOOcbIlOvbyGU49190862 = EeLOOcbIlOvbyGU89581019;     EeLOOcbIlOvbyGU89581019 = EeLOOcbIlOvbyGU40342706;     EeLOOcbIlOvbyGU40342706 = EeLOOcbIlOvbyGU73493132;     EeLOOcbIlOvbyGU73493132 = EeLOOcbIlOvbyGU28341478;     EeLOOcbIlOvbyGU28341478 = EeLOOcbIlOvbyGU31518994;     EeLOOcbIlOvbyGU31518994 = EeLOOcbIlOvbyGU89911754;     EeLOOcbIlOvbyGU89911754 = EeLOOcbIlOvbyGU85387249;     EeLOOcbIlOvbyGU85387249 = EeLOOcbIlOvbyGU6590257;     EeLOOcbIlOvbyGU6590257 = EeLOOcbIlOvbyGU58769992;     EeLOOcbIlOvbyGU58769992 = EeLOOcbIlOvbyGU62653458;     EeLOOcbIlOvbyGU62653458 = EeLOOcbIlOvbyGU29172373;     EeLOOcbIlOvbyGU29172373 = EeLOOcbIlOvbyGU44554281;     EeLOOcbIlOvbyGU44554281 = EeLOOcbIlOvbyGU47824195;     EeLOOcbIlOvbyGU47824195 = EeLOOcbIlOvbyGU38031832;     EeLOOcbIlOvbyGU38031832 = EeLOOcbIlOvbyGU41527705;     EeLOOcbIlOvbyGU41527705 = EeLOOcbIlOvbyGU75832104;     EeLOOcbIlOvbyGU75832104 = EeLOOcbIlOvbyGU98185501;     EeLOOcbIlOvbyGU98185501 = EeLOOcbIlOvbyGU29830181;     EeLOOcbIlOvbyGU29830181 = EeLOOcbIlOvbyGU31249021;     EeLOOcbIlOvbyGU31249021 = EeLOOcbIlOvbyGU65027802;     EeLOOcbIlOvbyGU65027802 = EeLOOcbIlOvbyGU22590586;     EeLOOcbIlOvbyGU22590586 = EeLOOcbIlOvbyGU52264256;     EeLOOcbIlOvbyGU52264256 = EeLOOcbIlOvbyGU73891496;     EeLOOcbIlOvbyGU73891496 = EeLOOcbIlOvbyGU28394086;     EeLOOcbIlOvbyGU28394086 = EeLOOcbIlOvbyGU24402416;     EeLOOcbIlOvbyGU24402416 = EeLOOcbIlOvbyGU39294424;     EeLOOcbIlOvbyGU39294424 = EeLOOcbIlOvbyGU28572024;     EeLOOcbIlOvbyGU28572024 = EeLOOcbIlOvbyGU78593995;     EeLOOcbIlOvbyGU78593995 = EeLOOcbIlOvbyGU35767860;     EeLOOcbIlOvbyGU35767860 = EeLOOcbIlOvbyGU34678314;     EeLOOcbIlOvbyGU34678314 = EeLOOcbIlOvbyGU2048403;     EeLOOcbIlOvbyGU2048403 = EeLOOcbIlOvbyGU28103435;     EeLOOcbIlOvbyGU28103435 = EeLOOcbIlOvbyGU79291958;     EeLOOcbIlOvbyGU79291958 = EeLOOcbIlOvbyGU10752951;     EeLOOcbIlOvbyGU10752951 = EeLOOcbIlOvbyGU67558925;     EeLOOcbIlOvbyGU67558925 = EeLOOcbIlOvbyGU8603352;     EeLOOcbIlOvbyGU8603352 = EeLOOcbIlOvbyGU36941177;     EeLOOcbIlOvbyGU36941177 = EeLOOcbIlOvbyGU27648369;     EeLOOcbIlOvbyGU27648369 = EeLOOcbIlOvbyGU87532424;     EeLOOcbIlOvbyGU87532424 = EeLOOcbIlOvbyGU4161580;     EeLOOcbIlOvbyGU4161580 = EeLOOcbIlOvbyGU85772432;     EeLOOcbIlOvbyGU85772432 = EeLOOcbIlOvbyGU78523172;     EeLOOcbIlOvbyGU78523172 = EeLOOcbIlOvbyGU81983114;     EeLOOcbIlOvbyGU81983114 = EeLOOcbIlOvbyGU91200353;     EeLOOcbIlOvbyGU91200353 = EeLOOcbIlOvbyGU86440984;     EeLOOcbIlOvbyGU86440984 = EeLOOcbIlOvbyGU9952253;     EeLOOcbIlOvbyGU9952253 = EeLOOcbIlOvbyGU29799416;     EeLOOcbIlOvbyGU29799416 = EeLOOcbIlOvbyGU31172007;     EeLOOcbIlOvbyGU31172007 = EeLOOcbIlOvbyGU68446534;     EeLOOcbIlOvbyGU68446534 = EeLOOcbIlOvbyGU13880313;     EeLOOcbIlOvbyGU13880313 = EeLOOcbIlOvbyGU45732348;     EeLOOcbIlOvbyGU45732348 = EeLOOcbIlOvbyGU20147621;     EeLOOcbIlOvbyGU20147621 = EeLOOcbIlOvbyGU33260803;     EeLOOcbIlOvbyGU33260803 = EeLOOcbIlOvbyGU78020314;     EeLOOcbIlOvbyGU78020314 = EeLOOcbIlOvbyGU26600276;     EeLOOcbIlOvbyGU26600276 = EeLOOcbIlOvbyGU37316763;     EeLOOcbIlOvbyGU37316763 = EeLOOcbIlOvbyGU66451210;     EeLOOcbIlOvbyGU66451210 = EeLOOcbIlOvbyGU45099046;     EeLOOcbIlOvbyGU45099046 = EeLOOcbIlOvbyGU3939062;     EeLOOcbIlOvbyGU3939062 = EeLOOcbIlOvbyGU92224570;     EeLOOcbIlOvbyGU92224570 = EeLOOcbIlOvbyGU61339730;     EeLOOcbIlOvbyGU61339730 = EeLOOcbIlOvbyGU6793254;     EeLOOcbIlOvbyGU6793254 = EeLOOcbIlOvbyGU70822396;     EeLOOcbIlOvbyGU70822396 = EeLOOcbIlOvbyGU24091679;     EeLOOcbIlOvbyGU24091679 = EeLOOcbIlOvbyGU60605055;     EeLOOcbIlOvbyGU60605055 = EeLOOcbIlOvbyGU1068938;     EeLOOcbIlOvbyGU1068938 = EeLOOcbIlOvbyGU65262323;     EeLOOcbIlOvbyGU65262323 = EeLOOcbIlOvbyGU37071244;     EeLOOcbIlOvbyGU37071244 = EeLOOcbIlOvbyGU70472907;     EeLOOcbIlOvbyGU70472907 = EeLOOcbIlOvbyGU32924353;     EeLOOcbIlOvbyGU32924353 = EeLOOcbIlOvbyGU38890928;     EeLOOcbIlOvbyGU38890928 = EeLOOcbIlOvbyGU70537133;     EeLOOcbIlOvbyGU70537133 = EeLOOcbIlOvbyGU42297756;     EeLOOcbIlOvbyGU42297756 = EeLOOcbIlOvbyGU25480277;}
// Junk Finished

// Junk Code By Peatreat & Thaisen's Gen
void rlYzLdKwXJVBSJLt25543406() {     int tnuhWFOgHcETNLj93206525 = -871438669;    int tnuhWFOgHcETNLj54518360 = -639764297;    int tnuhWFOgHcETNLj21376049 = -326656290;    int tnuhWFOgHcETNLj21264866 = -47753389;    int tnuhWFOgHcETNLj54902761 = -377374508;    int tnuhWFOgHcETNLj95457965 = 30593762;    int tnuhWFOgHcETNLj96742315 = -65679305;    int tnuhWFOgHcETNLj64618864 = -569760646;    int tnuhWFOgHcETNLj2563193 = -579588543;    int tnuhWFOgHcETNLj61760959 = 10875145;    int tnuhWFOgHcETNLj98302588 = -89976883;    int tnuhWFOgHcETNLj1503247 = -921393945;    int tnuhWFOgHcETNLj6203462 = -935926885;    int tnuhWFOgHcETNLj88605969 = -117489278;    int tnuhWFOgHcETNLj92436419 = -669080854;    int tnuhWFOgHcETNLj82906735 = -58624208;    int tnuhWFOgHcETNLj21457705 = -290630775;    int tnuhWFOgHcETNLj15886896 = -221576281;    int tnuhWFOgHcETNLj38112911 = -457009405;    int tnuhWFOgHcETNLj2159330 = -158490383;    int tnuhWFOgHcETNLj56352382 = -185484137;    int tnuhWFOgHcETNLj80571312 = -277273748;    int tnuhWFOgHcETNLj47619738 = -179524053;    int tnuhWFOgHcETNLj18699328 = -561164031;    int tnuhWFOgHcETNLj32740144 = -38533735;    int tnuhWFOgHcETNLj6684241 = 88156909;    int tnuhWFOgHcETNLj64511940 = -664449411;    int tnuhWFOgHcETNLj77646994 = -897780358;    int tnuhWFOgHcETNLj20016037 = -590072666;    int tnuhWFOgHcETNLj64304772 = -737279180;    int tnuhWFOgHcETNLj54339432 = -528792869;    int tnuhWFOgHcETNLj42792432 = -807513350;    int tnuhWFOgHcETNLj88746971 = -347622737;    int tnuhWFOgHcETNLj50402881 = -664339265;    int tnuhWFOgHcETNLj30673502 = 62699889;    int tnuhWFOgHcETNLj55964509 = -60751134;    int tnuhWFOgHcETNLj93330050 = -74313226;    int tnuhWFOgHcETNLj39738914 = -251567301;    int tnuhWFOgHcETNLj77406360 = -810418115;    int tnuhWFOgHcETNLj15896408 = 47258269;    int tnuhWFOgHcETNLj98685469 = -494193525;    int tnuhWFOgHcETNLj79059098 = -534143499;    int tnuhWFOgHcETNLj50665473 = -902818686;    int tnuhWFOgHcETNLj47867120 = -17544716;    int tnuhWFOgHcETNLj21847436 = -611601941;    int tnuhWFOgHcETNLj24443669 = -538881308;    int tnuhWFOgHcETNLj88549280 = -66886488;    int tnuhWFOgHcETNLj19145533 = -219154612;    int tnuhWFOgHcETNLj23039784 = 90147100;    int tnuhWFOgHcETNLj46179003 = 93078410;    int tnuhWFOgHcETNLj75409133 = -833877593;    int tnuhWFOgHcETNLj23418781 = -299404733;    int tnuhWFOgHcETNLj61329172 = -306513020;    int tnuhWFOgHcETNLj41693083 = -296608086;    int tnuhWFOgHcETNLj69191024 = 15194211;    int tnuhWFOgHcETNLj12635214 = -494164922;    int tnuhWFOgHcETNLj6898623 = -360240244;    int tnuhWFOgHcETNLj2676721 = -765492260;    int tnuhWFOgHcETNLj88524722 = 90780345;    int tnuhWFOgHcETNLj48218520 = -365531417;    int tnuhWFOgHcETNLj30946025 = -304956828;    int tnuhWFOgHcETNLj19095322 = -167898947;    int tnuhWFOgHcETNLj44602827 = -979687980;    int tnuhWFOgHcETNLj38258421 = -842309364;    int tnuhWFOgHcETNLj7421527 = -460331986;    int tnuhWFOgHcETNLj55510156 = -282463534;    int tnuhWFOgHcETNLj12756275 = -473771208;    int tnuhWFOgHcETNLj55800580 = -171587621;    int tnuhWFOgHcETNLj57932468 = -80189168;    int tnuhWFOgHcETNLj36471910 = -508329720;    int tnuhWFOgHcETNLj89576685 = -984310983;    int tnuhWFOgHcETNLj81718791 = 60936526;    int tnuhWFOgHcETNLj38480536 = -411158167;    int tnuhWFOgHcETNLj22216504 = -404267675;    int tnuhWFOgHcETNLj3473860 = -664296858;    int tnuhWFOgHcETNLj77293283 = -651340638;    int tnuhWFOgHcETNLj29905839 = -374455063;    int tnuhWFOgHcETNLj99752617 = -61979338;    int tnuhWFOgHcETNLj96851891 = -949562090;    int tnuhWFOgHcETNLj8296475 = -499652427;    int tnuhWFOgHcETNLj18134961 = -844956603;    int tnuhWFOgHcETNLj45366408 = -345294799;    int tnuhWFOgHcETNLj54607210 = -887927459;    int tnuhWFOgHcETNLj73837034 = -583151077;    int tnuhWFOgHcETNLj88895639 = -903401588;    int tnuhWFOgHcETNLj30920652 = -129388137;    int tnuhWFOgHcETNLj81463259 = -401000331;    int tnuhWFOgHcETNLj47053888 = 48985348;    int tnuhWFOgHcETNLj81211857 = -579533476;    int tnuhWFOgHcETNLj18038289 = -443135190;    int tnuhWFOgHcETNLj49065886 = -700510891;    int tnuhWFOgHcETNLj90653329 = -308820966;    int tnuhWFOgHcETNLj51214192 = -242347647;    int tnuhWFOgHcETNLj29187841 = -344886699;    int tnuhWFOgHcETNLj84950382 = -647784904;    int tnuhWFOgHcETNLj79590148 = -226294579;    int tnuhWFOgHcETNLj34456272 = -554455519;    int tnuhWFOgHcETNLj12407053 = 39490678;    int tnuhWFOgHcETNLj40445594 = -557212730;    int tnuhWFOgHcETNLj66337279 = -871438669;     tnuhWFOgHcETNLj93206525 = tnuhWFOgHcETNLj54518360;     tnuhWFOgHcETNLj54518360 = tnuhWFOgHcETNLj21376049;     tnuhWFOgHcETNLj21376049 = tnuhWFOgHcETNLj21264866;     tnuhWFOgHcETNLj21264866 = tnuhWFOgHcETNLj54902761;     tnuhWFOgHcETNLj54902761 = tnuhWFOgHcETNLj95457965;     tnuhWFOgHcETNLj95457965 = tnuhWFOgHcETNLj96742315;     tnuhWFOgHcETNLj96742315 = tnuhWFOgHcETNLj64618864;     tnuhWFOgHcETNLj64618864 = tnuhWFOgHcETNLj2563193;     tnuhWFOgHcETNLj2563193 = tnuhWFOgHcETNLj61760959;     tnuhWFOgHcETNLj61760959 = tnuhWFOgHcETNLj98302588;     tnuhWFOgHcETNLj98302588 = tnuhWFOgHcETNLj1503247;     tnuhWFOgHcETNLj1503247 = tnuhWFOgHcETNLj6203462;     tnuhWFOgHcETNLj6203462 = tnuhWFOgHcETNLj88605969;     tnuhWFOgHcETNLj88605969 = tnuhWFOgHcETNLj92436419;     tnuhWFOgHcETNLj92436419 = tnuhWFOgHcETNLj82906735;     tnuhWFOgHcETNLj82906735 = tnuhWFOgHcETNLj21457705;     tnuhWFOgHcETNLj21457705 = tnuhWFOgHcETNLj15886896;     tnuhWFOgHcETNLj15886896 = tnuhWFOgHcETNLj38112911;     tnuhWFOgHcETNLj38112911 = tnuhWFOgHcETNLj2159330;     tnuhWFOgHcETNLj2159330 = tnuhWFOgHcETNLj56352382;     tnuhWFOgHcETNLj56352382 = tnuhWFOgHcETNLj80571312;     tnuhWFOgHcETNLj80571312 = tnuhWFOgHcETNLj47619738;     tnuhWFOgHcETNLj47619738 = tnuhWFOgHcETNLj18699328;     tnuhWFOgHcETNLj18699328 = tnuhWFOgHcETNLj32740144;     tnuhWFOgHcETNLj32740144 = tnuhWFOgHcETNLj6684241;     tnuhWFOgHcETNLj6684241 = tnuhWFOgHcETNLj64511940;     tnuhWFOgHcETNLj64511940 = tnuhWFOgHcETNLj77646994;     tnuhWFOgHcETNLj77646994 = tnuhWFOgHcETNLj20016037;     tnuhWFOgHcETNLj20016037 = tnuhWFOgHcETNLj64304772;     tnuhWFOgHcETNLj64304772 = tnuhWFOgHcETNLj54339432;     tnuhWFOgHcETNLj54339432 = tnuhWFOgHcETNLj42792432;     tnuhWFOgHcETNLj42792432 = tnuhWFOgHcETNLj88746971;     tnuhWFOgHcETNLj88746971 = tnuhWFOgHcETNLj50402881;     tnuhWFOgHcETNLj50402881 = tnuhWFOgHcETNLj30673502;     tnuhWFOgHcETNLj30673502 = tnuhWFOgHcETNLj55964509;     tnuhWFOgHcETNLj55964509 = tnuhWFOgHcETNLj93330050;     tnuhWFOgHcETNLj93330050 = tnuhWFOgHcETNLj39738914;     tnuhWFOgHcETNLj39738914 = tnuhWFOgHcETNLj77406360;     tnuhWFOgHcETNLj77406360 = tnuhWFOgHcETNLj15896408;     tnuhWFOgHcETNLj15896408 = tnuhWFOgHcETNLj98685469;     tnuhWFOgHcETNLj98685469 = tnuhWFOgHcETNLj79059098;     tnuhWFOgHcETNLj79059098 = tnuhWFOgHcETNLj50665473;     tnuhWFOgHcETNLj50665473 = tnuhWFOgHcETNLj47867120;     tnuhWFOgHcETNLj47867120 = tnuhWFOgHcETNLj21847436;     tnuhWFOgHcETNLj21847436 = tnuhWFOgHcETNLj24443669;     tnuhWFOgHcETNLj24443669 = tnuhWFOgHcETNLj88549280;     tnuhWFOgHcETNLj88549280 = tnuhWFOgHcETNLj19145533;     tnuhWFOgHcETNLj19145533 = tnuhWFOgHcETNLj23039784;     tnuhWFOgHcETNLj23039784 = tnuhWFOgHcETNLj46179003;     tnuhWFOgHcETNLj46179003 = tnuhWFOgHcETNLj75409133;     tnuhWFOgHcETNLj75409133 = tnuhWFOgHcETNLj23418781;     tnuhWFOgHcETNLj23418781 = tnuhWFOgHcETNLj61329172;     tnuhWFOgHcETNLj61329172 = tnuhWFOgHcETNLj41693083;     tnuhWFOgHcETNLj41693083 = tnuhWFOgHcETNLj69191024;     tnuhWFOgHcETNLj69191024 = tnuhWFOgHcETNLj12635214;     tnuhWFOgHcETNLj12635214 = tnuhWFOgHcETNLj6898623;     tnuhWFOgHcETNLj6898623 = tnuhWFOgHcETNLj2676721;     tnuhWFOgHcETNLj2676721 = tnuhWFOgHcETNLj88524722;     tnuhWFOgHcETNLj88524722 = tnuhWFOgHcETNLj48218520;     tnuhWFOgHcETNLj48218520 = tnuhWFOgHcETNLj30946025;     tnuhWFOgHcETNLj30946025 = tnuhWFOgHcETNLj19095322;     tnuhWFOgHcETNLj19095322 = tnuhWFOgHcETNLj44602827;     tnuhWFOgHcETNLj44602827 = tnuhWFOgHcETNLj38258421;     tnuhWFOgHcETNLj38258421 = tnuhWFOgHcETNLj7421527;     tnuhWFOgHcETNLj7421527 = tnuhWFOgHcETNLj55510156;     tnuhWFOgHcETNLj55510156 = tnuhWFOgHcETNLj12756275;     tnuhWFOgHcETNLj12756275 = tnuhWFOgHcETNLj55800580;     tnuhWFOgHcETNLj55800580 = tnuhWFOgHcETNLj57932468;     tnuhWFOgHcETNLj57932468 = tnuhWFOgHcETNLj36471910;     tnuhWFOgHcETNLj36471910 = tnuhWFOgHcETNLj89576685;     tnuhWFOgHcETNLj89576685 = tnuhWFOgHcETNLj81718791;     tnuhWFOgHcETNLj81718791 = tnuhWFOgHcETNLj38480536;     tnuhWFOgHcETNLj38480536 = tnuhWFOgHcETNLj22216504;     tnuhWFOgHcETNLj22216504 = tnuhWFOgHcETNLj3473860;     tnuhWFOgHcETNLj3473860 = tnuhWFOgHcETNLj77293283;     tnuhWFOgHcETNLj77293283 = tnuhWFOgHcETNLj29905839;     tnuhWFOgHcETNLj29905839 = tnuhWFOgHcETNLj99752617;     tnuhWFOgHcETNLj99752617 = tnuhWFOgHcETNLj96851891;     tnuhWFOgHcETNLj96851891 = tnuhWFOgHcETNLj8296475;     tnuhWFOgHcETNLj8296475 = tnuhWFOgHcETNLj18134961;     tnuhWFOgHcETNLj18134961 = tnuhWFOgHcETNLj45366408;     tnuhWFOgHcETNLj45366408 = tnuhWFOgHcETNLj54607210;     tnuhWFOgHcETNLj54607210 = tnuhWFOgHcETNLj73837034;     tnuhWFOgHcETNLj73837034 = tnuhWFOgHcETNLj88895639;     tnuhWFOgHcETNLj88895639 = tnuhWFOgHcETNLj30920652;     tnuhWFOgHcETNLj30920652 = tnuhWFOgHcETNLj81463259;     tnuhWFOgHcETNLj81463259 = tnuhWFOgHcETNLj47053888;     tnuhWFOgHcETNLj47053888 = tnuhWFOgHcETNLj81211857;     tnuhWFOgHcETNLj81211857 = tnuhWFOgHcETNLj18038289;     tnuhWFOgHcETNLj18038289 = tnuhWFOgHcETNLj49065886;     tnuhWFOgHcETNLj49065886 = tnuhWFOgHcETNLj90653329;     tnuhWFOgHcETNLj90653329 = tnuhWFOgHcETNLj51214192;     tnuhWFOgHcETNLj51214192 = tnuhWFOgHcETNLj29187841;     tnuhWFOgHcETNLj29187841 = tnuhWFOgHcETNLj84950382;     tnuhWFOgHcETNLj84950382 = tnuhWFOgHcETNLj79590148;     tnuhWFOgHcETNLj79590148 = tnuhWFOgHcETNLj34456272;     tnuhWFOgHcETNLj34456272 = tnuhWFOgHcETNLj12407053;     tnuhWFOgHcETNLj12407053 = tnuhWFOgHcETNLj40445594;     tnuhWFOgHcETNLj40445594 = tnuhWFOgHcETNLj66337279;     tnuhWFOgHcETNLj66337279 = tnuhWFOgHcETNLj93206525;}
// Junk Finished

// Junk Code By Peatreat & Thaisen's Gen
void RBoGqoXVHnRWxVML79959619() {     float vFrIDmNhJYTHNBG63858192 = -389587799;    float vFrIDmNhJYTHNBG23191878 = -511407596;    float vFrIDmNhJYTHNBG1344968 = -768950750;    float vFrIDmNhJYTHNBG61196152 = -237891778;    float vFrIDmNhJYTHNBG76353082 = -160780855;    float vFrIDmNhJYTHNBG85457728 = -770987390;    float vFrIDmNhJYTHNBG15121212 = -64455015;    float vFrIDmNhJYTHNBG21461245 = -916577736;    float vFrIDmNhJYTHNBG48428247 = -672935872;    float vFrIDmNhJYTHNBG84727419 = -290220205;    float vFrIDmNhJYTHNBG40551749 = -124878812;    float vFrIDmNhJYTHNBG47049817 = -137157738;    float vFrIDmNhJYTHNBG86030848 = -845463944;    float vFrIDmNhJYTHNBG72453003 = 81482512;    float vFrIDmNhJYTHNBG75156438 = -818830857;    float vFrIDmNhJYTHNBG15435376 = -728607795;    float vFrIDmNhJYTHNBG15310532 = -199815905;    float vFrIDmNhJYTHNBG36180390 = -210742477;    float vFrIDmNhJYTHNBG19820693 = -64197347;    float vFrIDmNhJYTHNBG60946038 = -700856688;    float vFrIDmNhJYTHNBG11488137 = -94163277;    float vFrIDmNhJYTHNBG41967077 = -608559053;    float vFrIDmNhJYTHNBG49563546 = -747367198;    float vFrIDmNhJYTHNBG52548865 = -153564937;    float vFrIDmNhJYTHNBG96864531 = -814438423;    float vFrIDmNhJYTHNBG37281822 = -10127536;    float vFrIDmNhJYTHNBG72772726 = 40372814;    float vFrIDmNhJYTHNBG85429219 = -302904125;    float vFrIDmNhJYTHNBG52158525 = -884430883;    float vFrIDmNhJYTHNBG3476868 = -195674251;    float vFrIDmNhJYTHNBG61411429 = -277226555;    float vFrIDmNhJYTHNBG87303892 = -35810513;    float vFrIDmNhJYTHNBG71692332 = -294584346;    float vFrIDmNhJYTHNBG15767857 = -227868949;    float vFrIDmNhJYTHNBG8697739 = -884246890;    float vFrIDmNhJYTHNBG15957768 = -139977682;    float vFrIDmNhJYTHNBG37121536 = -262906433;    float vFrIDmNhJYTHNBG92531402 = -35680409;    float vFrIDmNhJYTHNBG55939649 = -766146752;    float vFrIDmNhJYTHNBG10999706 = -570350094;    float vFrIDmNhJYTHNBG58758754 = -878212404;    float vFrIDmNhJYTHNBG7542090 = -303532568;    float vFrIDmNhJYTHNBG99632433 = -889572738;    float vFrIDmNhJYTHNBG54375420 = -89519687;    float vFrIDmNhJYTHNBG17801495 = -920248336;    float vFrIDmNhJYTHNBG30849504 = -297218463;    float vFrIDmNhJYTHNBG14680484 = -182956288;    float vFrIDmNhJYTHNBG84759742 = -671284908;    float vFrIDmNhJYTHNBG28481237 = -995616869;    float vFrIDmNhJYTHNBG54606489 = -189930468;    float vFrIDmNhJYTHNBG43598457 = -380449714;    float vFrIDmNhJYTHNBG65290285 = -268624455;    float vFrIDmNhJYTHNBG16404829 = -234311062;    float vFrIDmNhJYTHNBG31920388 = -957489582;    float vFrIDmNhJYTHNBG68471483 = -95746704;    float vFrIDmNhJYTHNBG21891116 = -781028746;    float vFrIDmNhJYTHNBG73628331 = -764040399;    float vFrIDmNhJYTHNBG48796103 = -515385814;    float vFrIDmNhJYTHNBG64331620 = -423453356;    float vFrIDmNhJYTHNBG39071261 = -50653319;    float vFrIDmNhJYTHNBG12685002 = -711360204;    float vFrIDmNhJYTHNBG29691992 = -761550890;    float vFrIDmNhJYTHNBG69302720 = 67853147;    float vFrIDmNhJYTHNBG44951379 = -377261622;    float vFrIDmNhJYTHNBG23315991 = 87006349;    float vFrIDmNhJYTHNBG53247856 = 10931700;    float vFrIDmNhJYTHNBG75357485 = -842573392;    float vFrIDmNhJYTHNBG70262992 = -517594995;    float vFrIDmNhJYTHNBG63755265 = -34270598;    float vFrIDmNhJYTHNBG59198671 = -578853175;    float vFrIDmNhJYTHNBG78313839 = -365701362;    float vFrIDmNhJYTHNBG22779129 = -64135496;    float vFrIDmNhJYTHNBG80240741 = -444595726;    float vFrIDmNhJYTHNBG8820987 = -493847254;    float vFrIDmNhJYTHNBG2187285 = -822644285;    float vFrIDmNhJYTHNBG3946047 = -790630709;    float vFrIDmNhJYTHNBG42334644 = -718986316;    float vFrIDmNhJYTHNBG95188125 = -557847511;    float vFrIDmNhJYTHNBG34747370 = -233316602;    float vFrIDmNhJYTHNBG66015028 = -417219960;    float vFrIDmNhJYTHNBG22601339 = -827171249;    float vFrIDmNhJYTHNBG88012983 = -288342279;    float vFrIDmNhJYTHNBG56947983 = -307287257;    float vFrIDmNhJYTHNBG97552035 = -594500415;    float vFrIDmNhJYTHNBG59878411 = -815224537;    float vFrIDmNhJYTHNBG96121143 = 91397900;    float vFrIDmNhJYTHNBG70899063 = -801499452;    float vFrIDmNhJYTHNBG39771945 = -337094765;    float vFrIDmNhJYTHNBG47296374 = -32122246;    float vFrIDmNhJYTHNBG86806623 = -3218144;    float vFrIDmNhJYTHNBG42329436 = -375937284;    float vFrIDmNhJYTHNBG88325433 = -747520620;    float vFrIDmNhJYTHNBG28199783 = -612227054;    float vFrIDmNhJYTHNBG16868389 = -615493433;    float vFrIDmNhJYTHNBG98314703 = -858989890;    float vFrIDmNhJYTHNBG29066762 = -16661515;    float vFrIDmNhJYTHNBG38239370 = -271385716;    float vFrIDmNhJYTHNBG54681054 = -412311116;    float vFrIDmNhJYTHNBG31059430 = -76526037;    float vFrIDmNhJYTHNBG64553638 = -389587799;     vFrIDmNhJYTHNBG63858192 = vFrIDmNhJYTHNBG23191878;     vFrIDmNhJYTHNBG23191878 = vFrIDmNhJYTHNBG1344968;     vFrIDmNhJYTHNBG1344968 = vFrIDmNhJYTHNBG61196152;     vFrIDmNhJYTHNBG61196152 = vFrIDmNhJYTHNBG76353082;     vFrIDmNhJYTHNBG76353082 = vFrIDmNhJYTHNBG85457728;     vFrIDmNhJYTHNBG85457728 = vFrIDmNhJYTHNBG15121212;     vFrIDmNhJYTHNBG15121212 = vFrIDmNhJYTHNBG21461245;     vFrIDmNhJYTHNBG21461245 = vFrIDmNhJYTHNBG48428247;     vFrIDmNhJYTHNBG48428247 = vFrIDmNhJYTHNBG84727419;     vFrIDmNhJYTHNBG84727419 = vFrIDmNhJYTHNBG40551749;     vFrIDmNhJYTHNBG40551749 = vFrIDmNhJYTHNBG47049817;     vFrIDmNhJYTHNBG47049817 = vFrIDmNhJYTHNBG86030848;     vFrIDmNhJYTHNBG86030848 = vFrIDmNhJYTHNBG72453003;     vFrIDmNhJYTHNBG72453003 = vFrIDmNhJYTHNBG75156438;     vFrIDmNhJYTHNBG75156438 = vFrIDmNhJYTHNBG15435376;     vFrIDmNhJYTHNBG15435376 = vFrIDmNhJYTHNBG15310532;     vFrIDmNhJYTHNBG15310532 = vFrIDmNhJYTHNBG36180390;     vFrIDmNhJYTHNBG36180390 = vFrIDmNhJYTHNBG19820693;     vFrIDmNhJYTHNBG19820693 = vFrIDmNhJYTHNBG60946038;     vFrIDmNhJYTHNBG60946038 = vFrIDmNhJYTHNBG11488137;     vFrIDmNhJYTHNBG11488137 = vFrIDmNhJYTHNBG41967077;     vFrIDmNhJYTHNBG41967077 = vFrIDmNhJYTHNBG49563546;     vFrIDmNhJYTHNBG49563546 = vFrIDmNhJYTHNBG52548865;     vFrIDmNhJYTHNBG52548865 = vFrIDmNhJYTHNBG96864531;     vFrIDmNhJYTHNBG96864531 = vFrIDmNhJYTHNBG37281822;     vFrIDmNhJYTHNBG37281822 = vFrIDmNhJYTHNBG72772726;     vFrIDmNhJYTHNBG72772726 = vFrIDmNhJYTHNBG85429219;     vFrIDmNhJYTHNBG85429219 = vFrIDmNhJYTHNBG52158525;     vFrIDmNhJYTHNBG52158525 = vFrIDmNhJYTHNBG3476868;     vFrIDmNhJYTHNBG3476868 = vFrIDmNhJYTHNBG61411429;     vFrIDmNhJYTHNBG61411429 = vFrIDmNhJYTHNBG87303892;     vFrIDmNhJYTHNBG87303892 = vFrIDmNhJYTHNBG71692332;     vFrIDmNhJYTHNBG71692332 = vFrIDmNhJYTHNBG15767857;     vFrIDmNhJYTHNBG15767857 = vFrIDmNhJYTHNBG8697739;     vFrIDmNhJYTHNBG8697739 = vFrIDmNhJYTHNBG15957768;     vFrIDmNhJYTHNBG15957768 = vFrIDmNhJYTHNBG37121536;     vFrIDmNhJYTHNBG37121536 = vFrIDmNhJYTHNBG92531402;     vFrIDmNhJYTHNBG92531402 = vFrIDmNhJYTHNBG55939649;     vFrIDmNhJYTHNBG55939649 = vFrIDmNhJYTHNBG10999706;     vFrIDmNhJYTHNBG10999706 = vFrIDmNhJYTHNBG58758754;     vFrIDmNhJYTHNBG58758754 = vFrIDmNhJYTHNBG7542090;     vFrIDmNhJYTHNBG7542090 = vFrIDmNhJYTHNBG99632433;     vFrIDmNhJYTHNBG99632433 = vFrIDmNhJYTHNBG54375420;     vFrIDmNhJYTHNBG54375420 = vFrIDmNhJYTHNBG17801495;     vFrIDmNhJYTHNBG17801495 = vFrIDmNhJYTHNBG30849504;     vFrIDmNhJYTHNBG30849504 = vFrIDmNhJYTHNBG14680484;     vFrIDmNhJYTHNBG14680484 = vFrIDmNhJYTHNBG84759742;     vFrIDmNhJYTHNBG84759742 = vFrIDmNhJYTHNBG28481237;     vFrIDmNhJYTHNBG28481237 = vFrIDmNhJYTHNBG54606489;     vFrIDmNhJYTHNBG54606489 = vFrIDmNhJYTHNBG43598457;     vFrIDmNhJYTHNBG43598457 = vFrIDmNhJYTHNBG65290285;     vFrIDmNhJYTHNBG65290285 = vFrIDmNhJYTHNBG16404829;     vFrIDmNhJYTHNBG16404829 = vFrIDmNhJYTHNBG31920388;     vFrIDmNhJYTHNBG31920388 = vFrIDmNhJYTHNBG68471483;     vFrIDmNhJYTHNBG68471483 = vFrIDmNhJYTHNBG21891116;     vFrIDmNhJYTHNBG21891116 = vFrIDmNhJYTHNBG73628331;     vFrIDmNhJYTHNBG73628331 = vFrIDmNhJYTHNBG48796103;     vFrIDmNhJYTHNBG48796103 = vFrIDmNhJYTHNBG64331620;     vFrIDmNhJYTHNBG64331620 = vFrIDmNhJYTHNBG39071261;     vFrIDmNhJYTHNBG39071261 = vFrIDmNhJYTHNBG12685002;     vFrIDmNhJYTHNBG12685002 = vFrIDmNhJYTHNBG29691992;     vFrIDmNhJYTHNBG29691992 = vFrIDmNhJYTHNBG69302720;     vFrIDmNhJYTHNBG69302720 = vFrIDmNhJYTHNBG44951379;     vFrIDmNhJYTHNBG44951379 = vFrIDmNhJYTHNBG23315991;     vFrIDmNhJYTHNBG23315991 = vFrIDmNhJYTHNBG53247856;     vFrIDmNhJYTHNBG53247856 = vFrIDmNhJYTHNBG75357485;     vFrIDmNhJYTHNBG75357485 = vFrIDmNhJYTHNBG70262992;     vFrIDmNhJYTHNBG70262992 = vFrIDmNhJYTHNBG63755265;     vFrIDmNhJYTHNBG63755265 = vFrIDmNhJYTHNBG59198671;     vFrIDmNhJYTHNBG59198671 = vFrIDmNhJYTHNBG78313839;     vFrIDmNhJYTHNBG78313839 = vFrIDmNhJYTHNBG22779129;     vFrIDmNhJYTHNBG22779129 = vFrIDmNhJYTHNBG80240741;     vFrIDmNhJYTHNBG80240741 = vFrIDmNhJYTHNBG8820987;     vFrIDmNhJYTHNBG8820987 = vFrIDmNhJYTHNBG2187285;     vFrIDmNhJYTHNBG2187285 = vFrIDmNhJYTHNBG3946047;     vFrIDmNhJYTHNBG3946047 = vFrIDmNhJYTHNBG42334644;     vFrIDmNhJYTHNBG42334644 = vFrIDmNhJYTHNBG95188125;     vFrIDmNhJYTHNBG95188125 = vFrIDmNhJYTHNBG34747370;     vFrIDmNhJYTHNBG34747370 = vFrIDmNhJYTHNBG66015028;     vFrIDmNhJYTHNBG66015028 = vFrIDmNhJYTHNBG22601339;     vFrIDmNhJYTHNBG22601339 = vFrIDmNhJYTHNBG88012983;     vFrIDmNhJYTHNBG88012983 = vFrIDmNhJYTHNBG56947983;     vFrIDmNhJYTHNBG56947983 = vFrIDmNhJYTHNBG97552035;     vFrIDmNhJYTHNBG97552035 = vFrIDmNhJYTHNBG59878411;     vFrIDmNhJYTHNBG59878411 = vFrIDmNhJYTHNBG96121143;     vFrIDmNhJYTHNBG96121143 = vFrIDmNhJYTHNBG70899063;     vFrIDmNhJYTHNBG70899063 = vFrIDmNhJYTHNBG39771945;     vFrIDmNhJYTHNBG39771945 = vFrIDmNhJYTHNBG47296374;     vFrIDmNhJYTHNBG47296374 = vFrIDmNhJYTHNBG86806623;     vFrIDmNhJYTHNBG86806623 = vFrIDmNhJYTHNBG42329436;     vFrIDmNhJYTHNBG42329436 = vFrIDmNhJYTHNBG88325433;     vFrIDmNhJYTHNBG88325433 = vFrIDmNhJYTHNBG28199783;     vFrIDmNhJYTHNBG28199783 = vFrIDmNhJYTHNBG16868389;     vFrIDmNhJYTHNBG16868389 = vFrIDmNhJYTHNBG98314703;     vFrIDmNhJYTHNBG98314703 = vFrIDmNhJYTHNBG29066762;     vFrIDmNhJYTHNBG29066762 = vFrIDmNhJYTHNBG38239370;     vFrIDmNhJYTHNBG38239370 = vFrIDmNhJYTHNBG54681054;     vFrIDmNhJYTHNBG54681054 = vFrIDmNhJYTHNBG31059430;     vFrIDmNhJYTHNBG31059430 = vFrIDmNhJYTHNBG64553638;     vFrIDmNhJYTHNBG64553638 = vFrIDmNhJYTHNBG63858192;}
// Junk Finished

// Junk Code By Peatreat & Thaisen's Gen
void rNihLwJqexLnRqMW86618363() {     float YKVAlpvImeodAfF63851720 = -262111941;    float YKVAlpvImeodAfF61764194 = -554002721;    float YKVAlpvImeodAfF2658180 = -726079364;    float YKVAlpvImeodAfF46276013 = -58432608;    float YKVAlpvImeodAfF85193184 = -629136176;    float YKVAlpvImeodAfF20263161 = -470420201;    float YKVAlpvImeodAfF83668547 = -868991482;    float YKVAlpvImeodAfF90675295 = -620755053;    float YKVAlpvImeodAfF31168412 = -386581942;    float YKVAlpvImeodAfF81366933 = 72972924;    float YKVAlpvImeodAfF60872354 = -84350884;    float YKVAlpvImeodAfF61341494 = -972636100;    float YKVAlpvImeodAfF17883762 = -867122418;    float YKVAlpvImeodAfF26297224 = -263670540;    float YKVAlpvImeodAfF20328548 = -781543709;    float YKVAlpvImeodAfF89999923 = 38596555;    float YKVAlpvImeodAfF71163291 = -19430475;    float YKVAlpvImeodAfF59180923 = -607814099;    float YKVAlpvImeodAfF40248962 = -682179614;    float YKVAlpvImeodAfF33019802 = -212881139;    float YKVAlpvImeodAfF91157560 = -904629651;    float YKVAlpvImeodAfF20263343 = -18550937;    float YKVAlpvImeodAfF86441943 = -106325040;    float YKVAlpvImeodAfF96198973 = -34485460;    float YKVAlpvImeodAfF25307222 = -141132276;    float YKVAlpvImeodAfF53855899 = 81666536;    float YKVAlpvImeodAfF42994308 = -260426899;    float YKVAlpvImeodAfF20832745 = -911871114;    float YKVAlpvImeodAfF64836972 = -430501198;    float YKVAlpvImeodAfF79566738 = -971105624;    float YKVAlpvImeodAfF94824107 = -664319789;    float YKVAlpvImeodAfF72027750 = -427632274;    float YKVAlpvImeodAfF57144755 = -496528562;    float YKVAlpvImeodAfF16050694 = -104550682;    float YKVAlpvImeodAfF91055776 = -43560815;    float YKVAlpvImeodAfF58785957 = -219743429;    float YKVAlpvImeodAfF49062904 = -362559811;    float YKVAlpvImeodAfF62449082 = -30943088;    float YKVAlpvImeodAfF91253245 = -378828168;    float YKVAlpvImeodAfF82872598 = -179781780;    float YKVAlpvImeodAfF46614421 = -966156302;    float YKVAlpvImeodAfF93239750 = -497324651;    float YKVAlpvImeodAfF59915793 = -348702992;    float YKVAlpvImeodAfF13607319 = -746484131;    float YKVAlpvImeodAfF1819547 = -293507738;    float YKVAlpvImeodAfF71277472 = -109263134;    float YKVAlpvImeodAfF42378276 = -664236910;    float YKVAlpvImeodAfF60143323 = -180046264;    float YKVAlpvImeodAfF47139626 = -559380730;    float YKVAlpvImeodAfF74651912 = -876955355;    float YKVAlpvImeodAfF56857827 = 40995279;    float YKVAlpvImeodAfF79837542 = -650770203;    float YKVAlpvImeodAfF91853177 = -126365070;    float YKVAlpvImeodAfF42170658 = -389409434;    float YKVAlpvImeodAfF95460803 = -952645981;    float YKVAlpvImeodAfF43588378 = -143561005;    float YKVAlpvImeodAfF75322251 = -347677682;    float YKVAlpvImeodAfF6459207 = -591593905;    float YKVAlpvImeodAfF20968792 = -917300333;    float YKVAlpvImeodAfF31337286 = -610802712;    float YKVAlpvImeodAfF77268853 = -109993303;    float YKVAlpvImeodAfF62835803 = -957120368;    float YKVAlpvImeodAfF25838324 = -90253856;    float YKVAlpvImeodAfF51601673 = -415476318;    float YKVAlpvImeodAfF86542825 = -262707287;    float YKVAlpvImeodAfF88844604 = -656718611;    float YKVAlpvImeodAfF4196740 = -376107539;    float YKVAlpvImeodAfF1833069 = -662571737;    float YKVAlpvImeodAfF35241447 = -120109725;    float YKVAlpvImeodAfF61542590 = -461800281;    float YKVAlpvImeodAfF40937019 = -598843634;    float YKVAlpvImeodAfF8714209 = -988487387;    float YKVAlpvImeodAfF67927677 = -128985932;    float YKVAlpvImeodAfF57376364 = -402397835;    float YKVAlpvImeodAfF86405380 = -246724838;    float YKVAlpvImeodAfF97917810 = -307305000;    float YKVAlpvImeodAfF60347549 = -669847945;    float YKVAlpvImeodAfF72834624 = -359840910;    float YKVAlpvImeodAfF94379426 = -740977723;    float YKVAlpvImeodAfF54029750 = 68130857;    float YKVAlpvImeodAfF11477623 = -254096555;    float YKVAlpvImeodAfF82850984 = 19619364;    float YKVAlpvImeodAfF73693118 = -252490384;    float YKVAlpvImeodAfF90185059 = -553545844;    float YKVAlpvImeodAfF22708911 = -912100904;    float YKVAlpvImeodAfF14986566 = 86450413;    float YKVAlpvImeodAfF80174572 = -201267205;    float YKVAlpvImeodAfF14974097 = -7119129;    float YKVAlpvImeodAfF20589891 = -151904701;    float YKVAlpvImeodAfF47467398 = -899999811;    float YKVAlpvImeodAfF83463706 = -872065747;    float YKVAlpvImeodAfF42603698 = -770965907;    float YKVAlpvImeodAfF41480291 = -113642756;    float YKVAlpvImeodAfF59915960 = -768025456;    float YKVAlpvImeodAfF5603746 = 30211522;    float YKVAlpvImeodAfF83778618 = 90964066;    float YKVAlpvImeodAfF67401427 = -307070796;    float YKVAlpvImeodAfF8314120 = -933226674;    float YKVAlpvImeodAfF27064494 = -383776844;    float YKVAlpvImeodAfF12974943 = -262111941;     YKVAlpvImeodAfF63851720 = YKVAlpvImeodAfF61764194;     YKVAlpvImeodAfF61764194 = YKVAlpvImeodAfF2658180;     YKVAlpvImeodAfF2658180 = YKVAlpvImeodAfF46276013;     YKVAlpvImeodAfF46276013 = YKVAlpvImeodAfF85193184;     YKVAlpvImeodAfF85193184 = YKVAlpvImeodAfF20263161;     YKVAlpvImeodAfF20263161 = YKVAlpvImeodAfF83668547;     YKVAlpvImeodAfF83668547 = YKVAlpvImeodAfF90675295;     YKVAlpvImeodAfF90675295 = YKVAlpvImeodAfF31168412;     YKVAlpvImeodAfF31168412 = YKVAlpvImeodAfF81366933;     YKVAlpvImeodAfF81366933 = YKVAlpvImeodAfF60872354;     YKVAlpvImeodAfF60872354 = YKVAlpvImeodAfF61341494;     YKVAlpvImeodAfF61341494 = YKVAlpvImeodAfF17883762;     YKVAlpvImeodAfF17883762 = YKVAlpvImeodAfF26297224;     YKVAlpvImeodAfF26297224 = YKVAlpvImeodAfF20328548;     YKVAlpvImeodAfF20328548 = YKVAlpvImeodAfF89999923;     YKVAlpvImeodAfF89999923 = YKVAlpvImeodAfF71163291;     YKVAlpvImeodAfF71163291 = YKVAlpvImeodAfF59180923;     YKVAlpvImeodAfF59180923 = YKVAlpvImeodAfF40248962;     YKVAlpvImeodAfF40248962 = YKVAlpvImeodAfF33019802;     YKVAlpvImeodAfF33019802 = YKVAlpvImeodAfF91157560;     YKVAlpvImeodAfF91157560 = YKVAlpvImeodAfF20263343;     YKVAlpvImeodAfF20263343 = YKVAlpvImeodAfF86441943;     YKVAlpvImeodAfF86441943 = YKVAlpvImeodAfF96198973;     YKVAlpvImeodAfF96198973 = YKVAlpvImeodAfF25307222;     YKVAlpvImeodAfF25307222 = YKVAlpvImeodAfF53855899;     YKVAlpvImeodAfF53855899 = YKVAlpvImeodAfF42994308;     YKVAlpvImeodAfF42994308 = YKVAlpvImeodAfF20832745;     YKVAlpvImeodAfF20832745 = YKVAlpvImeodAfF64836972;     YKVAlpvImeodAfF64836972 = YKVAlpvImeodAfF79566738;     YKVAlpvImeodAfF79566738 = YKVAlpvImeodAfF94824107;     YKVAlpvImeodAfF94824107 = YKVAlpvImeodAfF72027750;     YKVAlpvImeodAfF72027750 = YKVAlpvImeodAfF57144755;     YKVAlpvImeodAfF57144755 = YKVAlpvImeodAfF16050694;     YKVAlpvImeodAfF16050694 = YKVAlpvImeodAfF91055776;     YKVAlpvImeodAfF91055776 = YKVAlpvImeodAfF58785957;     YKVAlpvImeodAfF58785957 = YKVAlpvImeodAfF49062904;     YKVAlpvImeodAfF49062904 = YKVAlpvImeodAfF62449082;     YKVAlpvImeodAfF62449082 = YKVAlpvImeodAfF91253245;     YKVAlpvImeodAfF91253245 = YKVAlpvImeodAfF82872598;     YKVAlpvImeodAfF82872598 = YKVAlpvImeodAfF46614421;     YKVAlpvImeodAfF46614421 = YKVAlpvImeodAfF93239750;     YKVAlpvImeodAfF93239750 = YKVAlpvImeodAfF59915793;     YKVAlpvImeodAfF59915793 = YKVAlpvImeodAfF13607319;     YKVAlpvImeodAfF13607319 = YKVAlpvImeodAfF1819547;     YKVAlpvImeodAfF1819547 = YKVAlpvImeodAfF71277472;     YKVAlpvImeodAfF71277472 = YKVAlpvImeodAfF42378276;     YKVAlpvImeodAfF42378276 = YKVAlpvImeodAfF60143323;     YKVAlpvImeodAfF60143323 = YKVAlpvImeodAfF47139626;     YKVAlpvImeodAfF47139626 = YKVAlpvImeodAfF74651912;     YKVAlpvImeodAfF74651912 = YKVAlpvImeodAfF56857827;     YKVAlpvImeodAfF56857827 = YKVAlpvImeodAfF79837542;     YKVAlpvImeodAfF79837542 = YKVAlpvImeodAfF91853177;     YKVAlpvImeodAfF91853177 = YKVAlpvImeodAfF42170658;     YKVAlpvImeodAfF42170658 = YKVAlpvImeodAfF95460803;     YKVAlpvImeodAfF95460803 = YKVAlpvImeodAfF43588378;     YKVAlpvImeodAfF43588378 = YKVAlpvImeodAfF75322251;     YKVAlpvImeodAfF75322251 = YKVAlpvImeodAfF6459207;     YKVAlpvImeodAfF6459207 = YKVAlpvImeodAfF20968792;     YKVAlpvImeodAfF20968792 = YKVAlpvImeodAfF31337286;     YKVAlpvImeodAfF31337286 = YKVAlpvImeodAfF77268853;     YKVAlpvImeodAfF77268853 = YKVAlpvImeodAfF62835803;     YKVAlpvImeodAfF62835803 = YKVAlpvImeodAfF25838324;     YKVAlpvImeodAfF25838324 = YKVAlpvImeodAfF51601673;     YKVAlpvImeodAfF51601673 = YKVAlpvImeodAfF86542825;     YKVAlpvImeodAfF86542825 = YKVAlpvImeodAfF88844604;     YKVAlpvImeodAfF88844604 = YKVAlpvImeodAfF4196740;     YKVAlpvImeodAfF4196740 = YKVAlpvImeodAfF1833069;     YKVAlpvImeodAfF1833069 = YKVAlpvImeodAfF35241447;     YKVAlpvImeodAfF35241447 = YKVAlpvImeodAfF61542590;     YKVAlpvImeodAfF61542590 = YKVAlpvImeodAfF40937019;     YKVAlpvImeodAfF40937019 = YKVAlpvImeodAfF8714209;     YKVAlpvImeodAfF8714209 = YKVAlpvImeodAfF67927677;     YKVAlpvImeodAfF67927677 = YKVAlpvImeodAfF57376364;     YKVAlpvImeodAfF57376364 = YKVAlpvImeodAfF86405380;     YKVAlpvImeodAfF86405380 = YKVAlpvImeodAfF97917810;     YKVAlpvImeodAfF97917810 = YKVAlpvImeodAfF60347549;     YKVAlpvImeodAfF60347549 = YKVAlpvImeodAfF72834624;     YKVAlpvImeodAfF72834624 = YKVAlpvImeodAfF94379426;     YKVAlpvImeodAfF94379426 = YKVAlpvImeodAfF54029750;     YKVAlpvImeodAfF54029750 = YKVAlpvImeodAfF11477623;     YKVAlpvImeodAfF11477623 = YKVAlpvImeodAfF82850984;     YKVAlpvImeodAfF82850984 = YKVAlpvImeodAfF73693118;     YKVAlpvImeodAfF73693118 = YKVAlpvImeodAfF90185059;     YKVAlpvImeodAfF90185059 = YKVAlpvImeodAfF22708911;     YKVAlpvImeodAfF22708911 = YKVAlpvImeodAfF14986566;     YKVAlpvImeodAfF14986566 = YKVAlpvImeodAfF80174572;     YKVAlpvImeodAfF80174572 = YKVAlpvImeodAfF14974097;     YKVAlpvImeodAfF14974097 = YKVAlpvImeodAfF20589891;     YKVAlpvImeodAfF20589891 = YKVAlpvImeodAfF47467398;     YKVAlpvImeodAfF47467398 = YKVAlpvImeodAfF83463706;     YKVAlpvImeodAfF83463706 = YKVAlpvImeodAfF42603698;     YKVAlpvImeodAfF42603698 = YKVAlpvImeodAfF41480291;     YKVAlpvImeodAfF41480291 = YKVAlpvImeodAfF59915960;     YKVAlpvImeodAfF59915960 = YKVAlpvImeodAfF5603746;     YKVAlpvImeodAfF5603746 = YKVAlpvImeodAfF83778618;     YKVAlpvImeodAfF83778618 = YKVAlpvImeodAfF67401427;     YKVAlpvImeodAfF67401427 = YKVAlpvImeodAfF8314120;     YKVAlpvImeodAfF8314120 = YKVAlpvImeodAfF27064494;     YKVAlpvImeodAfF27064494 = YKVAlpvImeodAfF12974943;     YKVAlpvImeodAfF12974943 = YKVAlpvImeodAfF63851720;}
// Junk Finished

// Junk Code By Peatreat & Thaisen's Gen
void hAkOfRnnJpmgNkGg62630484() {     float echyEjpujSUrZvT50773398 = -402763145;    float echyEjpujSUrZvT71411933 = -664589295;    float echyEjpujSUrZvT95354193 = -157194747;    float echyEjpujSUrZvT74139207 = -64229899;    float echyEjpujSUrZvT35922272 = -482949654;    float echyEjpujSUrZvT33728838 = -868113496;    float echyEjpujSUrZvT33714216 = -550789520;    float echyEjpujSUrZvT73391643 = -51294874;    float echyEjpujSUrZvT9554102 = -596092644;    float echyEjpujSUrZvT54867319 = -741888282;    float echyEjpujSUrZvT80553084 = -238439628;    float echyEjpujSUrZvT30967972 = 5261015;    float echyEjpujSUrZvT78510211 = -955485707;    float echyEjpujSUrZvT69615333 = -60168939;    float echyEjpujSUrZvT44041417 = -968309259;    float echyEjpujSUrZvT25279082 = 59944969;    float echyEjpujSUrZvT21003467 = -689350312;    float echyEjpujSUrZvT88397680 = -283200343;    float echyEjpujSUrZvT1408534 = -18700584;    float echyEjpujSUrZvT89772629 = -650978978;    float echyEjpujSUrZvT58623229 = -949308644;    float echyEjpujSUrZvT58953302 = -946672839;    float echyEjpujSUrZvT56088283 = -3731290;    float echyEjpujSUrZvT72555923 = 31425765;    float echyEjpujSUrZvT81272206 = -165400055;    float echyEjpujSUrZvT48034798 = -581856810;    float echyEjpujSUrZvT99884735 = -512528964;    float echyEjpujSUrZvT21419295 = -39520381;    float echyEjpujSUrZvT37739765 = -846733830;    float echyEjpujSUrZvT59280376 = -532325694;    float echyEjpujSUrZvT53944360 = -580748689;    float echyEjpujSUrZvT76469779 = -598553976;    float echyEjpujSUrZvT11417838 = 19779704;    float echyEjpujSUrZvT45973792 = -240665451;    float echyEjpujSUrZvT83834725 = -415530912;    float echyEjpujSUrZvT91746172 = -400339245;    float echyEjpujSUrZvT5032168 = -330465101;    float echyEjpujSUrZvT34777459 = 88824342;    float echyEjpujSUrZvT44484412 = -741679340;    float echyEjpujSUrZvT10659674 = -460174949;    float echyEjpujSUrZvT29775852 = -688078952;    float echyEjpujSUrZvT78080676 = -854480133;    float echyEjpujSUrZvT36365967 = -142183044;    float echyEjpujSUrZvT32151998 = -796479813;    float echyEjpujSUrZvT53804407 = -749399455;    float echyEjpujSUrZvT73844393 = -64613268;    float echyEjpujSUrZvT63028302 = -139941424;    float echyEjpujSUrZvT62399267 = -378816017;    float echyEjpujSUrZvT37365254 = -409124410;    float echyEjpujSUrZvT67251491 = -114973684;    float echyEjpujSUrZvT98215689 = -18359448;    float echyEjpujSUrZvT84750584 = -150082887;    float echyEjpujSUrZvT76994780 = 65715246;    float echyEjpujSUrZvT96715627 = 94498406;    float echyEjpujSUrZvT4007255 = -503759229;    float echyEjpujSUrZvT91820096 = -456090307;    float echyEjpujSUrZvT15323650 = -560858005;    float echyEjpujSUrZvT22798270 = -88620512;    float echyEjpujSUrZvT92867000 = -898829844;    float echyEjpujSUrZvT87887473 = -901092844;    float echyEjpujSUrZvT33844102 = -255584532;    float echyEjpujSUrZvT12294922 = -411269140;    float echyEjpujSUrZvT35651879 = -204561045;    float echyEjpujSUrZvT50273725 = 36233050;    float echyEjpujSUrZvT922960 = -61139594;    float echyEjpujSUrZvT4083305 = -639885652;    float echyEjpujSUrZvT19550134 = 85481310;    float echyEjpujSUrZvT32536420 = -614820257;    float echyEjpujSUrZvT85780607 = -644638028;    float echyEjpujSUrZvT52295245 = -467970014;    float echyEjpujSUrZvT20246915 = -609589931;    float echyEjpujSUrZvT86226007 = -678174654;    float echyEjpujSUrZvT43913268 = -541521004;    float echyEjpujSUrZvT90748859 = -558525636;    float echyEjpujSUrZvT59996777 = -962900026;    float echyEjpujSUrZvT80542553 = 5171489;    float echyEjpujSUrZvT22587335 = -704489796;    float echyEjpujSUrZvT23936285 = -207251477;    float echyEjpujSUrZvT18751517 = -219174780;    float echyEjpujSUrZvT7427813 = -786788;    float echyEjpujSUrZvT85006496 = -341915386;    float echyEjpujSUrZvT37485469 = -33712947;    float echyEjpujSUrZvT84054040 = -630395972;    float echyEjpujSUrZvT70488273 = -631760146;    float echyEjpujSUrZvT61064687 = -413966247;    float echyEjpujSUrZvT69193775 = -330665802;    float echyEjpujSUrZvT99474999 = -564269222;    float echyEjpujSUrZvT14702211 = 25281298;    float echyEjpujSUrZvT41966537 = -736906223;    float echyEjpujSUrZvT92014628 = -959440605;    float echyEjpujSUrZvT76422522 = -839481241;    float echyEjpujSUrZvT82233897 = -141844590;    float echyEjpujSUrZvT41910459 = -12345814;    float echyEjpujSUrZvT56596939 = -840586496;    float echyEjpujSUrZvT76815571 = -104590418;    float echyEjpujSUrZvT17480931 = -176809813;    float echyEjpujSUrZvT42428797 = -549919089;    float echyEjpujSUrZvT86092242 = -78416094;    float echyEjpujSUrZvT31229039 = -635340220;    float echyEjpujSUrZvT49721103 = -402763145;     echyEjpujSUrZvT50773398 = echyEjpujSUrZvT71411933;     echyEjpujSUrZvT71411933 = echyEjpujSUrZvT95354193;     echyEjpujSUrZvT95354193 = echyEjpujSUrZvT74139207;     echyEjpujSUrZvT74139207 = echyEjpujSUrZvT35922272;     echyEjpujSUrZvT35922272 = echyEjpujSUrZvT33728838;     echyEjpujSUrZvT33728838 = echyEjpujSUrZvT33714216;     echyEjpujSUrZvT33714216 = echyEjpujSUrZvT73391643;     echyEjpujSUrZvT73391643 = echyEjpujSUrZvT9554102;     echyEjpujSUrZvT9554102 = echyEjpujSUrZvT54867319;     echyEjpujSUrZvT54867319 = echyEjpujSUrZvT80553084;     echyEjpujSUrZvT80553084 = echyEjpujSUrZvT30967972;     echyEjpujSUrZvT30967972 = echyEjpujSUrZvT78510211;     echyEjpujSUrZvT78510211 = echyEjpujSUrZvT69615333;     echyEjpujSUrZvT69615333 = echyEjpujSUrZvT44041417;     echyEjpujSUrZvT44041417 = echyEjpujSUrZvT25279082;     echyEjpujSUrZvT25279082 = echyEjpujSUrZvT21003467;     echyEjpujSUrZvT21003467 = echyEjpujSUrZvT88397680;     echyEjpujSUrZvT88397680 = echyEjpujSUrZvT1408534;     echyEjpujSUrZvT1408534 = echyEjpujSUrZvT89772629;     echyEjpujSUrZvT89772629 = echyEjpujSUrZvT58623229;     echyEjpujSUrZvT58623229 = echyEjpujSUrZvT58953302;     echyEjpujSUrZvT58953302 = echyEjpujSUrZvT56088283;     echyEjpujSUrZvT56088283 = echyEjpujSUrZvT72555923;     echyEjpujSUrZvT72555923 = echyEjpujSUrZvT81272206;     echyEjpujSUrZvT81272206 = echyEjpujSUrZvT48034798;     echyEjpujSUrZvT48034798 = echyEjpujSUrZvT99884735;     echyEjpujSUrZvT99884735 = echyEjpujSUrZvT21419295;     echyEjpujSUrZvT21419295 = echyEjpujSUrZvT37739765;     echyEjpujSUrZvT37739765 = echyEjpujSUrZvT59280376;     echyEjpujSUrZvT59280376 = echyEjpujSUrZvT53944360;     echyEjpujSUrZvT53944360 = echyEjpujSUrZvT76469779;     echyEjpujSUrZvT76469779 = echyEjpujSUrZvT11417838;     echyEjpujSUrZvT11417838 = echyEjpujSUrZvT45973792;     echyEjpujSUrZvT45973792 = echyEjpujSUrZvT83834725;     echyEjpujSUrZvT83834725 = echyEjpujSUrZvT91746172;     echyEjpujSUrZvT91746172 = echyEjpujSUrZvT5032168;     echyEjpujSUrZvT5032168 = echyEjpujSUrZvT34777459;     echyEjpujSUrZvT34777459 = echyEjpujSUrZvT44484412;     echyEjpujSUrZvT44484412 = echyEjpujSUrZvT10659674;     echyEjpujSUrZvT10659674 = echyEjpujSUrZvT29775852;     echyEjpujSUrZvT29775852 = echyEjpujSUrZvT78080676;     echyEjpujSUrZvT78080676 = echyEjpujSUrZvT36365967;     echyEjpujSUrZvT36365967 = echyEjpujSUrZvT32151998;     echyEjpujSUrZvT32151998 = echyEjpujSUrZvT53804407;     echyEjpujSUrZvT53804407 = echyEjpujSUrZvT73844393;     echyEjpujSUrZvT73844393 = echyEjpujSUrZvT63028302;     echyEjpujSUrZvT63028302 = echyEjpujSUrZvT62399267;     echyEjpujSUrZvT62399267 = echyEjpujSUrZvT37365254;     echyEjpujSUrZvT37365254 = echyEjpujSUrZvT67251491;     echyEjpujSUrZvT67251491 = echyEjpujSUrZvT98215689;     echyEjpujSUrZvT98215689 = echyEjpujSUrZvT84750584;     echyEjpujSUrZvT84750584 = echyEjpujSUrZvT76994780;     echyEjpujSUrZvT76994780 = echyEjpujSUrZvT96715627;     echyEjpujSUrZvT96715627 = echyEjpujSUrZvT4007255;     echyEjpujSUrZvT4007255 = echyEjpujSUrZvT91820096;     echyEjpujSUrZvT91820096 = echyEjpujSUrZvT15323650;     echyEjpujSUrZvT15323650 = echyEjpujSUrZvT22798270;     echyEjpujSUrZvT22798270 = echyEjpujSUrZvT92867000;     echyEjpujSUrZvT92867000 = echyEjpujSUrZvT87887473;     echyEjpujSUrZvT87887473 = echyEjpujSUrZvT33844102;     echyEjpujSUrZvT33844102 = echyEjpujSUrZvT12294922;     echyEjpujSUrZvT12294922 = echyEjpujSUrZvT35651879;     echyEjpujSUrZvT35651879 = echyEjpujSUrZvT50273725;     echyEjpujSUrZvT50273725 = echyEjpujSUrZvT922960;     echyEjpujSUrZvT922960 = echyEjpujSUrZvT4083305;     echyEjpujSUrZvT4083305 = echyEjpujSUrZvT19550134;     echyEjpujSUrZvT19550134 = echyEjpujSUrZvT32536420;     echyEjpujSUrZvT32536420 = echyEjpujSUrZvT85780607;     echyEjpujSUrZvT85780607 = echyEjpujSUrZvT52295245;     echyEjpujSUrZvT52295245 = echyEjpujSUrZvT20246915;     echyEjpujSUrZvT20246915 = echyEjpujSUrZvT86226007;     echyEjpujSUrZvT86226007 = echyEjpujSUrZvT43913268;     echyEjpujSUrZvT43913268 = echyEjpujSUrZvT90748859;     echyEjpujSUrZvT90748859 = echyEjpujSUrZvT59996777;     echyEjpujSUrZvT59996777 = echyEjpujSUrZvT80542553;     echyEjpujSUrZvT80542553 = echyEjpujSUrZvT22587335;     echyEjpujSUrZvT22587335 = echyEjpujSUrZvT23936285;     echyEjpujSUrZvT23936285 = echyEjpujSUrZvT18751517;     echyEjpujSUrZvT18751517 = echyEjpujSUrZvT7427813;     echyEjpujSUrZvT7427813 = echyEjpujSUrZvT85006496;     echyEjpujSUrZvT85006496 = echyEjpujSUrZvT37485469;     echyEjpujSUrZvT37485469 = echyEjpujSUrZvT84054040;     echyEjpujSUrZvT84054040 = echyEjpujSUrZvT70488273;     echyEjpujSUrZvT70488273 = echyEjpujSUrZvT61064687;     echyEjpujSUrZvT61064687 = echyEjpujSUrZvT69193775;     echyEjpujSUrZvT69193775 = echyEjpujSUrZvT99474999;     echyEjpujSUrZvT99474999 = echyEjpujSUrZvT14702211;     echyEjpujSUrZvT14702211 = echyEjpujSUrZvT41966537;     echyEjpujSUrZvT41966537 = echyEjpujSUrZvT92014628;     echyEjpujSUrZvT92014628 = echyEjpujSUrZvT76422522;     echyEjpujSUrZvT76422522 = echyEjpujSUrZvT82233897;     echyEjpujSUrZvT82233897 = echyEjpujSUrZvT41910459;     echyEjpujSUrZvT41910459 = echyEjpujSUrZvT56596939;     echyEjpujSUrZvT56596939 = echyEjpujSUrZvT76815571;     echyEjpujSUrZvT76815571 = echyEjpujSUrZvT17480931;     echyEjpujSUrZvT17480931 = echyEjpujSUrZvT42428797;     echyEjpujSUrZvT42428797 = echyEjpujSUrZvT86092242;     echyEjpujSUrZvT86092242 = echyEjpujSUrZvT31229039;     echyEjpujSUrZvT31229039 = echyEjpujSUrZvT49721103;     echyEjpujSUrZvT49721103 = echyEjpujSUrZvT50773398;}
// Junk Finished

// Junk Code By Peatreat & Thaisen's Gen
void rxncXeppwhsCmufa47363124() {     float TpFykdeHSNGyALR89157785 = -217788655;    float TpFykdeHSNGyALR77435331 = 94789165;    float TpFykdeHSNGyALR43359745 = -453758437;    float TpFykdeHSNGyALR57642241 = -254673409;    float TpFykdeHSNGyALR81095176 = -84977763;    float TpFykdeHSNGyALR24437320 = -301152189;    float TpFykdeHSNGyALR23148147 = -69659864;    float TpFykdeHSNGyALR76692779 = -368140375;    float TpFykdeHSNGyALR64807876 = -526782641;    float TpFykdeHSNGyALR86965379 = -506923696;    float TpFykdeHSNGyALR81732809 = -744609386;    float TpFykdeHSNGyALR69652779 = -374823982;    float TpFykdeHSNGyALR61528464 = -580199781;    float TpFykdeHSNGyALR3110688 = -776802328;    float TpFykdeHSNGyALR33272641 = -838415343;    float TpFykdeHSNGyALR12296099 = -261546596;    float TpFykdeHSNGyALR64847881 = -402215433;    float TpFykdeHSNGyALR47071004 = -660544762;    float TpFykdeHSNGyALR23177345 = -575179104;    float TpFykdeHSNGyALR9441067 = -613591;    float TpFykdeHSNGyALR80467703 = -281391942;    float TpFykdeHSNGyALR32911696 = -516280350;    float TpFykdeHSNGyALR24855583 = -160911606;    float TpFykdeHSNGyALR31476879 = -425927182;    float TpFykdeHSNGyALR85184226 = -189950416;    float TpFykdeHSNGyALR25694427 = -20326694;    float TpFykdeHSNGyALR53245017 = -110448953;    float TpFykdeHSNGyALR81863970 = -325046742;    float TpFykdeHSNGyALR36877137 = -5104290;    float TpFykdeHSNGyALR13174243 = -720258663;    float TpFykdeHSNGyALR53601633 = -961626001;    float TpFykdeHSNGyALR47530821 = -67425965;    float TpFykdeHSNGyALR7745993 = 99992215;    float TpFykdeHSNGyALR76071561 = -448201175;    float TpFykdeHSNGyALR3584170 = -108370854;    float TpFykdeHSNGyALR6105759 = -546965573;    float TpFykdeHSNGyALR67558877 = -401579639;    float TpFykdeHSNGyALR28218810 = -788985216;    float TpFykdeHSNGyALR20556184 = -716505406;    float TpFykdeHSNGyALR1962292 = -455698742;    float TpFykdeHSNGyALR62647106 = -362725339;    float TpFykdeHSNGyALR58397400 = -874245807;    float TpFykdeHSNGyALR99882935 = -175962362;    float TpFykdeHSNGyALR29110019 = -292138768;    float TpFykdeHSNGyALR57757669 = -734671730;    float TpFykdeHSNGyALR33016909 = 63610097;    float TpFykdeHSNGyALR84983190 = -807364093;    float TpFykdeHSNGyALR49184842 = -609828931;    float TpFykdeHSNGyALR94923844 = -444874888;    float TpFykdeHSNGyALR27921061 = -299983528;    float TpFykdeHSNGyALR160692 = -262792344;    float TpFykdeHSNGyALR11091197 = -35055908;    float TpFykdeHSNGyALR78656837 = -894078569;    float TpFykdeHSNGyALR89813720 = -946177415;    float TpFykdeHSNGyALR66895421 = -359495577;    float TpFykdeHSNGyALR56246089 = -701508305;    float TpFykdeHSNGyALR52579748 = -744299230;    float TpFykdeHSNGyALR11882866 = 72168745;    float TpFykdeHSNGyALR72458015 = 35277006;    float TpFykdeHSNGyALR55400750 = 35348930;    float TpFykdeHSNGyALR71192302 = -90703236;    float TpFykdeHSNGyALR41284177 = -744613123;    float TpFykdeHSNGyALR39815643 = -263036086;    float TpFykdeHSNGyALR51633633 = -806523978;    float TpFykdeHSNGyALR33363746 = -545297696;    float TpFykdeHSNGyALR34201989 = -577183421;    float TpFykdeHSNGyALR61906786 = -374816197;    float TpFykdeHSNGyALR85456903 = -31998606;    float TpFykdeHSNGyALR99526518 = -568431474;    float TpFykdeHSNGyALR27166882 = -191449771;    float TpFykdeHSNGyALR44737221 = -859966957;    float TpFykdeHSNGyALR36629072 = -613230217;    float TpFykdeHSNGyALR26514821 = -944039357;    float TpFykdeHSNGyALR21215054 = -19480363;    float TpFykdeHSNGyALR46793960 = -637888252;    float TpFykdeHSNGyALR22070303 = -407146136;    float TpFykdeHSNGyALR33028760 = -240317989;    float TpFykdeHSNGyALR95745564 = -868772838;    float TpFykdeHSNGyALR73719210 = -691255453;    float TpFykdeHSNGyALR52167317 = -153560513;    float TpFykdeHSNGyALR40711236 = -212962602;    float TpFykdeHSNGyALR4060175 = -500620022;    float TpFykdeHSNGyALR86940125 = -880171854;    float TpFykdeHSNGyALR8956076 = -705120763;    float TpFykdeHSNGyALR13013552 = -357466319;    float TpFykdeHSNGyALR42510436 = -826570093;    float TpFykdeHSNGyALR68873983 = -173347397;    float TpFykdeHSNGyALR17932273 = 46169629;    float TpFykdeHSNGyALR9176140 = 11294401;    float TpFykdeHSNGyALR47338081 = -406862549;    float TpFykdeHSNGyALR53526010 = -802666344;    float TpFykdeHSNGyALR55676012 = -373748385;    float TpFykdeHSNGyALR55760795 = -724262223;    float TpFykdeHSNGyALR65155433 = -651854337;    float TpFykdeHSNGyALR30769989 = -264995506;    float TpFykdeHSNGyALR21362930 = -618112217;    float TpFykdeHSNGyALR18581758 = -511209722;    float TpFykdeHSNGyALR48249303 = -369438384;    float TpFykdeHSNGyALR95746272 = -746841072;    float TpFykdeHSNGyALR23555680 = -217788655;     TpFykdeHSNGyALR89157785 = TpFykdeHSNGyALR77435331;     TpFykdeHSNGyALR77435331 = TpFykdeHSNGyALR43359745;     TpFykdeHSNGyALR43359745 = TpFykdeHSNGyALR57642241;     TpFykdeHSNGyALR57642241 = TpFykdeHSNGyALR81095176;     TpFykdeHSNGyALR81095176 = TpFykdeHSNGyALR24437320;     TpFykdeHSNGyALR24437320 = TpFykdeHSNGyALR23148147;     TpFykdeHSNGyALR23148147 = TpFykdeHSNGyALR76692779;     TpFykdeHSNGyALR76692779 = TpFykdeHSNGyALR64807876;     TpFykdeHSNGyALR64807876 = TpFykdeHSNGyALR86965379;     TpFykdeHSNGyALR86965379 = TpFykdeHSNGyALR81732809;     TpFykdeHSNGyALR81732809 = TpFykdeHSNGyALR69652779;     TpFykdeHSNGyALR69652779 = TpFykdeHSNGyALR61528464;     TpFykdeHSNGyALR61528464 = TpFykdeHSNGyALR3110688;     TpFykdeHSNGyALR3110688 = TpFykdeHSNGyALR33272641;     TpFykdeHSNGyALR33272641 = TpFykdeHSNGyALR12296099;     TpFykdeHSNGyALR12296099 = TpFykdeHSNGyALR64847881;     TpFykdeHSNGyALR64847881 = TpFykdeHSNGyALR47071004;     TpFykdeHSNGyALR47071004 = TpFykdeHSNGyALR23177345;     TpFykdeHSNGyALR23177345 = TpFykdeHSNGyALR9441067;     TpFykdeHSNGyALR9441067 = TpFykdeHSNGyALR80467703;     TpFykdeHSNGyALR80467703 = TpFykdeHSNGyALR32911696;     TpFykdeHSNGyALR32911696 = TpFykdeHSNGyALR24855583;     TpFykdeHSNGyALR24855583 = TpFykdeHSNGyALR31476879;     TpFykdeHSNGyALR31476879 = TpFykdeHSNGyALR85184226;     TpFykdeHSNGyALR85184226 = TpFykdeHSNGyALR25694427;     TpFykdeHSNGyALR25694427 = TpFykdeHSNGyALR53245017;     TpFykdeHSNGyALR53245017 = TpFykdeHSNGyALR81863970;     TpFykdeHSNGyALR81863970 = TpFykdeHSNGyALR36877137;     TpFykdeHSNGyALR36877137 = TpFykdeHSNGyALR13174243;     TpFykdeHSNGyALR13174243 = TpFykdeHSNGyALR53601633;     TpFykdeHSNGyALR53601633 = TpFykdeHSNGyALR47530821;     TpFykdeHSNGyALR47530821 = TpFykdeHSNGyALR7745993;     TpFykdeHSNGyALR7745993 = TpFykdeHSNGyALR76071561;     TpFykdeHSNGyALR76071561 = TpFykdeHSNGyALR3584170;     TpFykdeHSNGyALR3584170 = TpFykdeHSNGyALR6105759;     TpFykdeHSNGyALR6105759 = TpFykdeHSNGyALR67558877;     TpFykdeHSNGyALR67558877 = TpFykdeHSNGyALR28218810;     TpFykdeHSNGyALR28218810 = TpFykdeHSNGyALR20556184;     TpFykdeHSNGyALR20556184 = TpFykdeHSNGyALR1962292;     TpFykdeHSNGyALR1962292 = TpFykdeHSNGyALR62647106;     TpFykdeHSNGyALR62647106 = TpFykdeHSNGyALR58397400;     TpFykdeHSNGyALR58397400 = TpFykdeHSNGyALR99882935;     TpFykdeHSNGyALR99882935 = TpFykdeHSNGyALR29110019;     TpFykdeHSNGyALR29110019 = TpFykdeHSNGyALR57757669;     TpFykdeHSNGyALR57757669 = TpFykdeHSNGyALR33016909;     TpFykdeHSNGyALR33016909 = TpFykdeHSNGyALR84983190;     TpFykdeHSNGyALR84983190 = TpFykdeHSNGyALR49184842;     TpFykdeHSNGyALR49184842 = TpFykdeHSNGyALR94923844;     TpFykdeHSNGyALR94923844 = TpFykdeHSNGyALR27921061;     TpFykdeHSNGyALR27921061 = TpFykdeHSNGyALR160692;     TpFykdeHSNGyALR160692 = TpFykdeHSNGyALR11091197;     TpFykdeHSNGyALR11091197 = TpFykdeHSNGyALR78656837;     TpFykdeHSNGyALR78656837 = TpFykdeHSNGyALR89813720;     TpFykdeHSNGyALR89813720 = TpFykdeHSNGyALR66895421;     TpFykdeHSNGyALR66895421 = TpFykdeHSNGyALR56246089;     TpFykdeHSNGyALR56246089 = TpFykdeHSNGyALR52579748;     TpFykdeHSNGyALR52579748 = TpFykdeHSNGyALR11882866;     TpFykdeHSNGyALR11882866 = TpFykdeHSNGyALR72458015;     TpFykdeHSNGyALR72458015 = TpFykdeHSNGyALR55400750;     TpFykdeHSNGyALR55400750 = TpFykdeHSNGyALR71192302;     TpFykdeHSNGyALR71192302 = TpFykdeHSNGyALR41284177;     TpFykdeHSNGyALR41284177 = TpFykdeHSNGyALR39815643;     TpFykdeHSNGyALR39815643 = TpFykdeHSNGyALR51633633;     TpFykdeHSNGyALR51633633 = TpFykdeHSNGyALR33363746;     TpFykdeHSNGyALR33363746 = TpFykdeHSNGyALR34201989;     TpFykdeHSNGyALR34201989 = TpFykdeHSNGyALR61906786;     TpFykdeHSNGyALR61906786 = TpFykdeHSNGyALR85456903;     TpFykdeHSNGyALR85456903 = TpFykdeHSNGyALR99526518;     TpFykdeHSNGyALR99526518 = TpFykdeHSNGyALR27166882;     TpFykdeHSNGyALR27166882 = TpFykdeHSNGyALR44737221;     TpFykdeHSNGyALR44737221 = TpFykdeHSNGyALR36629072;     TpFykdeHSNGyALR36629072 = TpFykdeHSNGyALR26514821;     TpFykdeHSNGyALR26514821 = TpFykdeHSNGyALR21215054;     TpFykdeHSNGyALR21215054 = TpFykdeHSNGyALR46793960;     TpFykdeHSNGyALR46793960 = TpFykdeHSNGyALR22070303;     TpFykdeHSNGyALR22070303 = TpFykdeHSNGyALR33028760;     TpFykdeHSNGyALR33028760 = TpFykdeHSNGyALR95745564;     TpFykdeHSNGyALR95745564 = TpFykdeHSNGyALR73719210;     TpFykdeHSNGyALR73719210 = TpFykdeHSNGyALR52167317;     TpFykdeHSNGyALR52167317 = TpFykdeHSNGyALR40711236;     TpFykdeHSNGyALR40711236 = TpFykdeHSNGyALR4060175;     TpFykdeHSNGyALR4060175 = TpFykdeHSNGyALR86940125;     TpFykdeHSNGyALR86940125 = TpFykdeHSNGyALR8956076;     TpFykdeHSNGyALR8956076 = TpFykdeHSNGyALR13013552;     TpFykdeHSNGyALR13013552 = TpFykdeHSNGyALR42510436;     TpFykdeHSNGyALR42510436 = TpFykdeHSNGyALR68873983;     TpFykdeHSNGyALR68873983 = TpFykdeHSNGyALR17932273;     TpFykdeHSNGyALR17932273 = TpFykdeHSNGyALR9176140;     TpFykdeHSNGyALR9176140 = TpFykdeHSNGyALR47338081;     TpFykdeHSNGyALR47338081 = TpFykdeHSNGyALR53526010;     TpFykdeHSNGyALR53526010 = TpFykdeHSNGyALR55676012;     TpFykdeHSNGyALR55676012 = TpFykdeHSNGyALR55760795;     TpFykdeHSNGyALR55760795 = TpFykdeHSNGyALR65155433;     TpFykdeHSNGyALR65155433 = TpFykdeHSNGyALR30769989;     TpFykdeHSNGyALR30769989 = TpFykdeHSNGyALR21362930;     TpFykdeHSNGyALR21362930 = TpFykdeHSNGyALR18581758;     TpFykdeHSNGyALR18581758 = TpFykdeHSNGyALR48249303;     TpFykdeHSNGyALR48249303 = TpFykdeHSNGyALR95746272;     TpFykdeHSNGyALR95746272 = TpFykdeHSNGyALR23555680;     TpFykdeHSNGyALR23555680 = TpFykdeHSNGyALR89157785;}
// Junk Finished

// Junk Code By Peatreat & Thaisen's Gen
void iIBVghGptWGtQqrH54021869() {     float rhOHUsnArogPWui89151313 = -90312794;    float rhOHUsnArogPWui16007648 = 52194046;    float rhOHUsnArogPWui44672957 = -410887051;    float rhOHUsnArogPWui42722102 = -75214242;    float rhOHUsnArogPWui89935278 = -553333076;    float rhOHUsnArogPWui59242752 = -585001;    float rhOHUsnArogPWui91695482 = -874196331;    float rhOHUsnArogPWui45906830 = -72317677;    float rhOHUsnArogPWui47548040 = -240428710;    float rhOHUsnArogPWui83604892 = -143730567;    float rhOHUsnArogPWui2053416 = -704081457;    float rhOHUsnArogPWui83944456 = -110302336;    float rhOHUsnArogPWui93381377 = -601858255;    float rhOHUsnArogPWui56954908 = -21955378;    float rhOHUsnArogPWui78444749 = -801128177;    float rhOHUsnArogPWui86860645 = -594342247;    float rhOHUsnArogPWui20700641 = -221830003;    float rhOHUsnArogPWui70071537 = 42383620;    float rhOHUsnArogPWui43605614 = -93161372;    float rhOHUsnArogPWui81514829 = -612638042;    float rhOHUsnArogPWui60137127 = 8141689;    float rhOHUsnArogPWui11207962 = 73727767;    float rhOHUsnArogPWui61733979 = -619869448;    float rhOHUsnArogPWui75126987 = -306847705;    float rhOHUsnArogPWui13626916 = -616644256;    float rhOHUsnArogPWui42268503 = 71467374;    float rhOHUsnArogPWui23466599 = -411248666;    float rhOHUsnArogPWui17267495 = -934013724;    float rhOHUsnArogPWui49555584 = -651174621;    float rhOHUsnArogPWui89264112 = -395690037;    float rhOHUsnArogPWui87014311 = -248719235;    float rhOHUsnArogPWui32254679 = -459247730;    float rhOHUsnArogPWui93198415 = -101952004;    float rhOHUsnArogPWui76354398 = -324882908;    float rhOHUsnArogPWui85942207 = -367684782;    float rhOHUsnArogPWui48933948 = -626731338;    float rhOHUsnArogPWui79500245 = -501233018;    float rhOHUsnArogPWui98136489 = -784247892;    float rhOHUsnArogPWui55869780 = -329186835;    float rhOHUsnArogPWui73835184 = -65130428;    float rhOHUsnArogPWui50502774 = -450669237;    float rhOHUsnArogPWui44095061 = 31962090;    float rhOHUsnArogPWui60166296 = -735092618;    float rhOHUsnArogPWui88341916 = -949103211;    float rhOHUsnArogPWui41775721 = -107931133;    float rhOHUsnArogPWui73444877 = -848434593;    float rhOHUsnArogPWui12680984 = -188644715;    float rhOHUsnArogPWui24568424 = -118590288;    float rhOHUsnArogPWui13582235 = -8638774;    float rhOHUsnArogPWui47966484 = -987008409;    float rhOHUsnArogPWui13420061 = -941347352;    float rhOHUsnArogPWui25638454 = -417201660;    float rhOHUsnArogPWui54105186 = -786132574;    float rhOHUsnArogPWui63991 = -378097266;    float rhOHUsnArogPWui93884741 = -116394860;    float rhOHUsnArogPWui77943351 = -64040562;    float rhOHUsnArogPWui54273669 = -327936507;    float rhOHUsnArogPWui69545970 = -4039346;    float rhOHUsnArogPWui29095186 = -458569986;    float rhOHUsnArogPWui47666776 = -524800450;    float rhOHUsnArogPWui35776153 = -589336335;    float rhOHUsnArogPWui74427987 = -940182607;    float rhOHUsnArogPWui96351246 = -421143057;    float rhOHUsnArogPWui58283927 = -844738674;    float rhOHUsnArogPWui96590581 = -895011333;    float rhOHUsnArogPWui69798737 = -144833727;    float rhOHUsnArogPWui90746040 = 91649668;    float rhOHUsnArogPWui17026980 = -176975348;    float rhOHUsnArogPWui71012700 = -654270597;    float rhOHUsnArogPWui29510802 = -74396840;    float rhOHUsnArogPWui7360401 = 6890770;    float rhOHUsnArogPWui22564152 = -437582111;    float rhOHUsnArogPWui14201757 = -628429546;    float rhOHUsnArogPWui69770429 = 71969056;    float rhOHUsnArogPWui31012056 = -61968805;    float rhOHUsnArogPWui16042066 = 76179599;    float rhOHUsnArogPWui51041666 = -191179616;    float rhOHUsnArogPWui73392062 = -670766237;    float rhOHUsnArogPWui33351266 = -98916573;    float rhOHUsnArogPWui40182039 = -768209664;    float rhOHUsnArogPWui29587519 = -739887912;    float rhOHUsnArogPWui98898175 = -192658378;    float rhOHUsnArogPWui3685261 = -825374950;    float rhOHUsnArogPWui1589100 = -664166212;    float rhOHUsnArogPWui75844052 = -454342686;    float rhOHUsnArogPWui61375857 = -831517575;    float rhOHUsnArogPWui78149492 = -673115156;    float rhOHUsnArogPWui93134424 = -723854739;    float rhOHUsnArogPWui82469656 = -108488049;    float rhOHUsnArogPWui7998856 = -203644221;    float rhOHUsnArogPWui94660279 = -198794832;    float rhOHUsnArogPWui9954276 = -397193672;    float rhOHUsnArogPWui69041303 = -225677907;    float rhOHUsnArogPWui8203005 = -804386385;    float rhOHUsnArogPWui38059031 = -475794093;    float rhOHUsnArogPWui76074786 = -510486630;    float rhOHUsnArogPWui47743815 = -546894853;    float rhOHUsnArogPWui1882369 = -890353945;    float rhOHUsnArogPWui91751335 = 45908121;    float rhOHUsnArogPWui71976984 = -90312794;     rhOHUsnArogPWui89151313 = rhOHUsnArogPWui16007648;     rhOHUsnArogPWui16007648 = rhOHUsnArogPWui44672957;     rhOHUsnArogPWui44672957 = rhOHUsnArogPWui42722102;     rhOHUsnArogPWui42722102 = rhOHUsnArogPWui89935278;     rhOHUsnArogPWui89935278 = rhOHUsnArogPWui59242752;     rhOHUsnArogPWui59242752 = rhOHUsnArogPWui91695482;     rhOHUsnArogPWui91695482 = rhOHUsnArogPWui45906830;     rhOHUsnArogPWui45906830 = rhOHUsnArogPWui47548040;     rhOHUsnArogPWui47548040 = rhOHUsnArogPWui83604892;     rhOHUsnArogPWui83604892 = rhOHUsnArogPWui2053416;     rhOHUsnArogPWui2053416 = rhOHUsnArogPWui83944456;     rhOHUsnArogPWui83944456 = rhOHUsnArogPWui93381377;     rhOHUsnArogPWui93381377 = rhOHUsnArogPWui56954908;     rhOHUsnArogPWui56954908 = rhOHUsnArogPWui78444749;     rhOHUsnArogPWui78444749 = rhOHUsnArogPWui86860645;     rhOHUsnArogPWui86860645 = rhOHUsnArogPWui20700641;     rhOHUsnArogPWui20700641 = rhOHUsnArogPWui70071537;     rhOHUsnArogPWui70071537 = rhOHUsnArogPWui43605614;     rhOHUsnArogPWui43605614 = rhOHUsnArogPWui81514829;     rhOHUsnArogPWui81514829 = rhOHUsnArogPWui60137127;     rhOHUsnArogPWui60137127 = rhOHUsnArogPWui11207962;     rhOHUsnArogPWui11207962 = rhOHUsnArogPWui61733979;     rhOHUsnArogPWui61733979 = rhOHUsnArogPWui75126987;     rhOHUsnArogPWui75126987 = rhOHUsnArogPWui13626916;     rhOHUsnArogPWui13626916 = rhOHUsnArogPWui42268503;     rhOHUsnArogPWui42268503 = rhOHUsnArogPWui23466599;     rhOHUsnArogPWui23466599 = rhOHUsnArogPWui17267495;     rhOHUsnArogPWui17267495 = rhOHUsnArogPWui49555584;     rhOHUsnArogPWui49555584 = rhOHUsnArogPWui89264112;     rhOHUsnArogPWui89264112 = rhOHUsnArogPWui87014311;     rhOHUsnArogPWui87014311 = rhOHUsnArogPWui32254679;     rhOHUsnArogPWui32254679 = rhOHUsnArogPWui93198415;     rhOHUsnArogPWui93198415 = rhOHUsnArogPWui76354398;     rhOHUsnArogPWui76354398 = rhOHUsnArogPWui85942207;     rhOHUsnArogPWui85942207 = rhOHUsnArogPWui48933948;     rhOHUsnArogPWui48933948 = rhOHUsnArogPWui79500245;     rhOHUsnArogPWui79500245 = rhOHUsnArogPWui98136489;     rhOHUsnArogPWui98136489 = rhOHUsnArogPWui55869780;     rhOHUsnArogPWui55869780 = rhOHUsnArogPWui73835184;     rhOHUsnArogPWui73835184 = rhOHUsnArogPWui50502774;     rhOHUsnArogPWui50502774 = rhOHUsnArogPWui44095061;     rhOHUsnArogPWui44095061 = rhOHUsnArogPWui60166296;     rhOHUsnArogPWui60166296 = rhOHUsnArogPWui88341916;     rhOHUsnArogPWui88341916 = rhOHUsnArogPWui41775721;     rhOHUsnArogPWui41775721 = rhOHUsnArogPWui73444877;     rhOHUsnArogPWui73444877 = rhOHUsnArogPWui12680984;     rhOHUsnArogPWui12680984 = rhOHUsnArogPWui24568424;     rhOHUsnArogPWui24568424 = rhOHUsnArogPWui13582235;     rhOHUsnArogPWui13582235 = rhOHUsnArogPWui47966484;     rhOHUsnArogPWui47966484 = rhOHUsnArogPWui13420061;     rhOHUsnArogPWui13420061 = rhOHUsnArogPWui25638454;     rhOHUsnArogPWui25638454 = rhOHUsnArogPWui54105186;     rhOHUsnArogPWui54105186 = rhOHUsnArogPWui63991;     rhOHUsnArogPWui63991 = rhOHUsnArogPWui93884741;     rhOHUsnArogPWui93884741 = rhOHUsnArogPWui77943351;     rhOHUsnArogPWui77943351 = rhOHUsnArogPWui54273669;     rhOHUsnArogPWui54273669 = rhOHUsnArogPWui69545970;     rhOHUsnArogPWui69545970 = rhOHUsnArogPWui29095186;     rhOHUsnArogPWui29095186 = rhOHUsnArogPWui47666776;     rhOHUsnArogPWui47666776 = rhOHUsnArogPWui35776153;     rhOHUsnArogPWui35776153 = rhOHUsnArogPWui74427987;     rhOHUsnArogPWui74427987 = rhOHUsnArogPWui96351246;     rhOHUsnArogPWui96351246 = rhOHUsnArogPWui58283927;     rhOHUsnArogPWui58283927 = rhOHUsnArogPWui96590581;     rhOHUsnArogPWui96590581 = rhOHUsnArogPWui69798737;     rhOHUsnArogPWui69798737 = rhOHUsnArogPWui90746040;     rhOHUsnArogPWui90746040 = rhOHUsnArogPWui17026980;     rhOHUsnArogPWui17026980 = rhOHUsnArogPWui71012700;     rhOHUsnArogPWui71012700 = rhOHUsnArogPWui29510802;     rhOHUsnArogPWui29510802 = rhOHUsnArogPWui7360401;     rhOHUsnArogPWui7360401 = rhOHUsnArogPWui22564152;     rhOHUsnArogPWui22564152 = rhOHUsnArogPWui14201757;     rhOHUsnArogPWui14201757 = rhOHUsnArogPWui69770429;     rhOHUsnArogPWui69770429 = rhOHUsnArogPWui31012056;     rhOHUsnArogPWui31012056 = rhOHUsnArogPWui16042066;     rhOHUsnArogPWui16042066 = rhOHUsnArogPWui51041666;     rhOHUsnArogPWui51041666 = rhOHUsnArogPWui73392062;     rhOHUsnArogPWui73392062 = rhOHUsnArogPWui33351266;     rhOHUsnArogPWui33351266 = rhOHUsnArogPWui40182039;     rhOHUsnArogPWui40182039 = rhOHUsnArogPWui29587519;     rhOHUsnArogPWui29587519 = rhOHUsnArogPWui98898175;     rhOHUsnArogPWui98898175 = rhOHUsnArogPWui3685261;     rhOHUsnArogPWui3685261 = rhOHUsnArogPWui1589100;     rhOHUsnArogPWui1589100 = rhOHUsnArogPWui75844052;     rhOHUsnArogPWui75844052 = rhOHUsnArogPWui61375857;     rhOHUsnArogPWui61375857 = rhOHUsnArogPWui78149492;     rhOHUsnArogPWui78149492 = rhOHUsnArogPWui93134424;     rhOHUsnArogPWui93134424 = rhOHUsnArogPWui82469656;     rhOHUsnArogPWui82469656 = rhOHUsnArogPWui7998856;     rhOHUsnArogPWui7998856 = rhOHUsnArogPWui94660279;     rhOHUsnArogPWui94660279 = rhOHUsnArogPWui9954276;     rhOHUsnArogPWui9954276 = rhOHUsnArogPWui69041303;     rhOHUsnArogPWui69041303 = rhOHUsnArogPWui8203005;     rhOHUsnArogPWui8203005 = rhOHUsnArogPWui38059031;     rhOHUsnArogPWui38059031 = rhOHUsnArogPWui76074786;     rhOHUsnArogPWui76074786 = rhOHUsnArogPWui47743815;     rhOHUsnArogPWui47743815 = rhOHUsnArogPWui1882369;     rhOHUsnArogPWui1882369 = rhOHUsnArogPWui91751335;     rhOHUsnArogPWui91751335 = rhOHUsnArogPWui71976984;     rhOHUsnArogPWui71976984 = rhOHUsnArogPWui89151313;}
// Junk Finished

// Junk Code By Peatreat & Thaisen's Gen
void WIYELgKfARaQOepI8438082() {     float VbAZqBBLyLaIWmu59802981 = -708461927;    float VbAZqBBLyLaIWmu84681165 = -919449259;    float VbAZqBBLyLaIWmu24641876 = -853181511;    float VbAZqBBLyLaIWmu82653388 = -265352628;    float VbAZqBBLyLaIWmu11385600 = -336739432;    float VbAZqBBLyLaIWmu49242515 = -802166152;    float VbAZqBBLyLaIWmu10074378 = -872972041;    float VbAZqBBLyLaIWmu2749211 = -419134782;    float VbAZqBBLyLaIWmu93413094 = -333776039;    float VbAZqBBLyLaIWmu6571354 = -444825918;    float VbAZqBBLyLaIWmu44302576 = -738983387;    float VbAZqBBLyLaIWmu29491027 = -426066138;    float VbAZqBBLyLaIWmu73208765 = -511395314;    float VbAZqBBLyLaIWmu40801942 = -922983589;    float VbAZqBBLyLaIWmu61164769 = -950878199;    float VbAZqBBLyLaIWmu19389287 = -164325834;    float VbAZqBBLyLaIWmu14553468 = -131015133;    float VbAZqBBLyLaIWmu90365031 = 53217420;    float VbAZqBBLyLaIWmu25313397 = -800349313;    float VbAZqBBLyLaIWmu40301538 = -55004347;    float VbAZqBBLyLaIWmu15272882 = 99462544;    float VbAZqBBLyLaIWmu72603727 = -257557539;    float VbAZqBBLyLaIWmu63677788 = -87712592;    float VbAZqBBLyLaIWmu8976524 = -999248611;    float VbAZqBBLyLaIWmu77751304 = -292548956;    float VbAZqBBLyLaIWmu72866084 = -26817067;    float VbAZqBBLyLaIWmu31727385 = -806426441;    float VbAZqBBLyLaIWmu25049720 = -339137498;    float VbAZqBBLyLaIWmu81698071 = -945532822;    float VbAZqBBLyLaIWmu28436209 = -954085107;    float VbAZqBBLyLaIWmu94086308 = 2847079;    float VbAZqBBLyLaIWmu76766139 = -787544890;    float VbAZqBBLyLaIWmu76143776 = -48913610;    float VbAZqBBLyLaIWmu41719373 = -988412592;    float VbAZqBBLyLaIWmu63966444 = -214631558;    float VbAZqBBLyLaIWmu8927208 = -705957867;    float VbAZqBBLyLaIWmu23291732 = -689826225;    float VbAZqBBLyLaIWmu50928978 = -568361003;    float VbAZqBBLyLaIWmu34403069 = -284915459;    float VbAZqBBLyLaIWmu68938482 = -682738791;    float VbAZqBBLyLaIWmu10576058 = -834688115;    float VbAZqBBLyLaIWmu72578052 = -837426959;    float VbAZqBBLyLaIWmu9133256 = -721846668;    float VbAZqBBLyLaIWmu94850217 = 78921817;    float VbAZqBBLyLaIWmu37729780 = -416577526;    float VbAZqBBLyLaIWmu79850711 = -606771729;    float VbAZqBBLyLaIWmu38812186 = -304714514;    float VbAZqBBLyLaIWmu90182633 = -570720583;    float VbAZqBBLyLaIWmu19023687 = 5597281;    float VbAZqBBLyLaIWmu56393970 = -170017293;    float VbAZqBBLyLaIWmu81609385 = -487919473;    float VbAZqBBLyLaIWmu67509959 = -386421378;    float VbAZqBBLyLaIWmu9180843 = -713930619;    float VbAZqBBLyLaIWmu90291295 = 61021237;    float VbAZqBBLyLaIWmu93165200 = -227335769;    float VbAZqBBLyLaIWmu87199254 = -350904389;    float VbAZqBBLyLaIWmu21003377 = -731736667;    float VbAZqBBLyLaIWmu15665352 = -853932900;    float VbAZqBBLyLaIWmu4902085 = -972803672;    float VbAZqBBLyLaIWmu38519516 = -209922365;    float VbAZqBBLyLaIWmu17515130 = -995739711;    float VbAZqBBLyLaIWmu85024658 = -433834544;    float VbAZqBBLyLaIWmu21051139 = -473601961;    float VbAZqBBLyLaIWmu64976886 = -379690933;    float VbAZqBBLyLaIWmu12485046 = -347672998;    float VbAZqBBLyLaIWmu67536437 = -951438498;    float VbAZqBBLyLaIWmu53347250 = -277152528;    float VbAZqBBLyLaIWmu31489392 = -522982722;    float VbAZqBBLyLaIWmu76835498 = -608352032;    float VbAZqBBLyLaIWmu52237562 = -144920332;    float VbAZqBBLyLaIWmu96097555 = -474499609;    float VbAZqBBLyLaIWmu63624490 = -562654130;    float VbAZqBBLyLaIWmu55961962 = -661867122;    float VbAZqBBLyLaIWmu56374914 = -17610522;    float VbAZqBBLyLaIWmu29725481 = -220316232;    float VbAZqBBLyLaIWmu42694829 = -63110498;    float VbAZqBBLyLaIWmu63470471 = -535710871;    float VbAZqBBLyLaIWmu68827571 = -66634410;    float VbAZqBBLyLaIWmu71246744 = -482671085;    float VbAZqBBLyLaIWmu97900592 = -685777228;    float VbAZqBBLyLaIWmu34053898 = -722102553;    float VbAZqBBLyLaIWmu41544751 = -135705859;    float VbAZqBBLyLaIWmu6026034 = -244734779;    float VbAZqBBLyLaIWmu25304101 = -675515530;    float VbAZqBBLyLaIWmu46826824 = -366165635;    float VbAZqBBLyLaIWmu26576350 = -610731543;    float VbAZqBBLyLaIWmu67585296 = 26385729;    float VbAZqBBLyLaIWmu85852481 = -9934848;    float VbAZqBBLyLaIWmu48554173 = -661076824;    float VbAZqBBLyLaIWmu76767190 = -863727170;    float VbAZqBBLyLaIWmu87923830 = -974221200;    float VbAZqBBLyLaIWmu7626380 = -835893326;    float VbAZqBBLyLaIWmu46026894 = -595557331;    float VbAZqBBLyLaIWmu95883553 = 25006906;    float VbAZqBBLyLaIWmu51423352 = -686999080;    float VbAZqBBLyLaIWmu25551400 = -300853572;    float VbAZqBBLyLaIWmu51526913 = -263824998;    float VbAZqBBLyLaIWmu44156370 = -242155736;    float VbAZqBBLyLaIWmu82365172 = -573405186;    float VbAZqBBLyLaIWmu70193343 = -708461927;     VbAZqBBLyLaIWmu59802981 = VbAZqBBLyLaIWmu84681165;     VbAZqBBLyLaIWmu84681165 = VbAZqBBLyLaIWmu24641876;     VbAZqBBLyLaIWmu24641876 = VbAZqBBLyLaIWmu82653388;     VbAZqBBLyLaIWmu82653388 = VbAZqBBLyLaIWmu11385600;     VbAZqBBLyLaIWmu11385600 = VbAZqBBLyLaIWmu49242515;     VbAZqBBLyLaIWmu49242515 = VbAZqBBLyLaIWmu10074378;     VbAZqBBLyLaIWmu10074378 = VbAZqBBLyLaIWmu2749211;     VbAZqBBLyLaIWmu2749211 = VbAZqBBLyLaIWmu93413094;     VbAZqBBLyLaIWmu93413094 = VbAZqBBLyLaIWmu6571354;     VbAZqBBLyLaIWmu6571354 = VbAZqBBLyLaIWmu44302576;     VbAZqBBLyLaIWmu44302576 = VbAZqBBLyLaIWmu29491027;     VbAZqBBLyLaIWmu29491027 = VbAZqBBLyLaIWmu73208765;     VbAZqBBLyLaIWmu73208765 = VbAZqBBLyLaIWmu40801942;     VbAZqBBLyLaIWmu40801942 = VbAZqBBLyLaIWmu61164769;     VbAZqBBLyLaIWmu61164769 = VbAZqBBLyLaIWmu19389287;     VbAZqBBLyLaIWmu19389287 = VbAZqBBLyLaIWmu14553468;     VbAZqBBLyLaIWmu14553468 = VbAZqBBLyLaIWmu90365031;     VbAZqBBLyLaIWmu90365031 = VbAZqBBLyLaIWmu25313397;     VbAZqBBLyLaIWmu25313397 = VbAZqBBLyLaIWmu40301538;     VbAZqBBLyLaIWmu40301538 = VbAZqBBLyLaIWmu15272882;     VbAZqBBLyLaIWmu15272882 = VbAZqBBLyLaIWmu72603727;     VbAZqBBLyLaIWmu72603727 = VbAZqBBLyLaIWmu63677788;     VbAZqBBLyLaIWmu63677788 = VbAZqBBLyLaIWmu8976524;     VbAZqBBLyLaIWmu8976524 = VbAZqBBLyLaIWmu77751304;     VbAZqBBLyLaIWmu77751304 = VbAZqBBLyLaIWmu72866084;     VbAZqBBLyLaIWmu72866084 = VbAZqBBLyLaIWmu31727385;     VbAZqBBLyLaIWmu31727385 = VbAZqBBLyLaIWmu25049720;     VbAZqBBLyLaIWmu25049720 = VbAZqBBLyLaIWmu81698071;     VbAZqBBLyLaIWmu81698071 = VbAZqBBLyLaIWmu28436209;     VbAZqBBLyLaIWmu28436209 = VbAZqBBLyLaIWmu94086308;     VbAZqBBLyLaIWmu94086308 = VbAZqBBLyLaIWmu76766139;     VbAZqBBLyLaIWmu76766139 = VbAZqBBLyLaIWmu76143776;     VbAZqBBLyLaIWmu76143776 = VbAZqBBLyLaIWmu41719373;     VbAZqBBLyLaIWmu41719373 = VbAZqBBLyLaIWmu63966444;     VbAZqBBLyLaIWmu63966444 = VbAZqBBLyLaIWmu8927208;     VbAZqBBLyLaIWmu8927208 = VbAZqBBLyLaIWmu23291732;     VbAZqBBLyLaIWmu23291732 = VbAZqBBLyLaIWmu50928978;     VbAZqBBLyLaIWmu50928978 = VbAZqBBLyLaIWmu34403069;     VbAZqBBLyLaIWmu34403069 = VbAZqBBLyLaIWmu68938482;     VbAZqBBLyLaIWmu68938482 = VbAZqBBLyLaIWmu10576058;     VbAZqBBLyLaIWmu10576058 = VbAZqBBLyLaIWmu72578052;     VbAZqBBLyLaIWmu72578052 = VbAZqBBLyLaIWmu9133256;     VbAZqBBLyLaIWmu9133256 = VbAZqBBLyLaIWmu94850217;     VbAZqBBLyLaIWmu94850217 = VbAZqBBLyLaIWmu37729780;     VbAZqBBLyLaIWmu37729780 = VbAZqBBLyLaIWmu79850711;     VbAZqBBLyLaIWmu79850711 = VbAZqBBLyLaIWmu38812186;     VbAZqBBLyLaIWmu38812186 = VbAZqBBLyLaIWmu90182633;     VbAZqBBLyLaIWmu90182633 = VbAZqBBLyLaIWmu19023687;     VbAZqBBLyLaIWmu19023687 = VbAZqBBLyLaIWmu56393970;     VbAZqBBLyLaIWmu56393970 = VbAZqBBLyLaIWmu81609385;     VbAZqBBLyLaIWmu81609385 = VbAZqBBLyLaIWmu67509959;     VbAZqBBLyLaIWmu67509959 = VbAZqBBLyLaIWmu9180843;     VbAZqBBLyLaIWmu9180843 = VbAZqBBLyLaIWmu90291295;     VbAZqBBLyLaIWmu90291295 = VbAZqBBLyLaIWmu93165200;     VbAZqBBLyLaIWmu93165200 = VbAZqBBLyLaIWmu87199254;     VbAZqBBLyLaIWmu87199254 = VbAZqBBLyLaIWmu21003377;     VbAZqBBLyLaIWmu21003377 = VbAZqBBLyLaIWmu15665352;     VbAZqBBLyLaIWmu15665352 = VbAZqBBLyLaIWmu4902085;     VbAZqBBLyLaIWmu4902085 = VbAZqBBLyLaIWmu38519516;     VbAZqBBLyLaIWmu38519516 = VbAZqBBLyLaIWmu17515130;     VbAZqBBLyLaIWmu17515130 = VbAZqBBLyLaIWmu85024658;     VbAZqBBLyLaIWmu85024658 = VbAZqBBLyLaIWmu21051139;     VbAZqBBLyLaIWmu21051139 = VbAZqBBLyLaIWmu64976886;     VbAZqBBLyLaIWmu64976886 = VbAZqBBLyLaIWmu12485046;     VbAZqBBLyLaIWmu12485046 = VbAZqBBLyLaIWmu67536437;     VbAZqBBLyLaIWmu67536437 = VbAZqBBLyLaIWmu53347250;     VbAZqBBLyLaIWmu53347250 = VbAZqBBLyLaIWmu31489392;     VbAZqBBLyLaIWmu31489392 = VbAZqBBLyLaIWmu76835498;     VbAZqBBLyLaIWmu76835498 = VbAZqBBLyLaIWmu52237562;     VbAZqBBLyLaIWmu52237562 = VbAZqBBLyLaIWmu96097555;     VbAZqBBLyLaIWmu96097555 = VbAZqBBLyLaIWmu63624490;     VbAZqBBLyLaIWmu63624490 = VbAZqBBLyLaIWmu55961962;     VbAZqBBLyLaIWmu55961962 = VbAZqBBLyLaIWmu56374914;     VbAZqBBLyLaIWmu56374914 = VbAZqBBLyLaIWmu29725481;     VbAZqBBLyLaIWmu29725481 = VbAZqBBLyLaIWmu42694829;     VbAZqBBLyLaIWmu42694829 = VbAZqBBLyLaIWmu63470471;     VbAZqBBLyLaIWmu63470471 = VbAZqBBLyLaIWmu68827571;     VbAZqBBLyLaIWmu68827571 = VbAZqBBLyLaIWmu71246744;     VbAZqBBLyLaIWmu71246744 = VbAZqBBLyLaIWmu97900592;     VbAZqBBLyLaIWmu97900592 = VbAZqBBLyLaIWmu34053898;     VbAZqBBLyLaIWmu34053898 = VbAZqBBLyLaIWmu41544751;     VbAZqBBLyLaIWmu41544751 = VbAZqBBLyLaIWmu6026034;     VbAZqBBLyLaIWmu6026034 = VbAZqBBLyLaIWmu25304101;     VbAZqBBLyLaIWmu25304101 = VbAZqBBLyLaIWmu46826824;     VbAZqBBLyLaIWmu46826824 = VbAZqBBLyLaIWmu26576350;     VbAZqBBLyLaIWmu26576350 = VbAZqBBLyLaIWmu67585296;     VbAZqBBLyLaIWmu67585296 = VbAZqBBLyLaIWmu85852481;     VbAZqBBLyLaIWmu85852481 = VbAZqBBLyLaIWmu48554173;     VbAZqBBLyLaIWmu48554173 = VbAZqBBLyLaIWmu76767190;     VbAZqBBLyLaIWmu76767190 = VbAZqBBLyLaIWmu87923830;     VbAZqBBLyLaIWmu87923830 = VbAZqBBLyLaIWmu7626380;     VbAZqBBLyLaIWmu7626380 = VbAZqBBLyLaIWmu46026894;     VbAZqBBLyLaIWmu46026894 = VbAZqBBLyLaIWmu95883553;     VbAZqBBLyLaIWmu95883553 = VbAZqBBLyLaIWmu51423352;     VbAZqBBLyLaIWmu51423352 = VbAZqBBLyLaIWmu25551400;     VbAZqBBLyLaIWmu25551400 = VbAZqBBLyLaIWmu51526913;     VbAZqBBLyLaIWmu51526913 = VbAZqBBLyLaIWmu44156370;     VbAZqBBLyLaIWmu44156370 = VbAZqBBLyLaIWmu82365172;     VbAZqBBLyLaIWmu82365172 = VbAZqBBLyLaIWmu70193343;     VbAZqBBLyLaIWmu70193343 = VbAZqBBLyLaIWmu59802981;}
// Junk Finished

// Junk Code By Peatreat & Thaisen's Gen
void LWNqTILMbRsBdTGG25894780() {     float UCMYZtHqGsLvCLD17931515 = -892237105;    float UCMYZtHqGsLvCLD93740592 = -531516021;    float UCMYZtHqGsLvCLD32318636 = -304720586;    float UCMYZtHqGsLvCLD11699203 = -543722909;    float UCMYZtHqGsLvCLD34865085 = -290298318;    float UCMYZtHqGsLvCLD95780904 = -299655035;    float UCMYZtHqGsLvCLD94455100 = -419019672;    float UCMYZtHqGsLvCLD34900245 = -215173465;    float UCMYZtHqGsLvCLD42413577 = -655503796;    float UCMYZtHqGsLvCLD28477830 = -338515717;    float UCMYZtHqGsLvCLD3338966 = -208048866;    float UCMYZtHqGsLvCLD5822658 = -614714046;    float UCMYZtHqGsLvCLD95461209 = -72466903;    float UCMYZtHqGsLvCLD74381699 = -715871735;    float UCMYZtHqGsLvCLD76833303 = -932098824;    float UCMYZtHqGsLvCLD45329093 = -701455484;    float UCMYZtHqGsLvCLD98399901 = -330997056;    float UCMYZtHqGsLvCLD67827196 = -186964226;    float UCMYZtHqGsLvCLD85467560 = -732998093;    float UCMYZtHqGsLvCLD61358361 = -614894565;    float UCMYZtHqGsLvCLD1107263 = -779003756;    float UCMYZtHqGsLvCLD39547090 = 34032279;    float UCMYZtHqGsLvCLD84407450 = -211451987;    float UCMYZtHqGsLvCLD73880339 = -501013068;    float UCMYZtHqGsLvCLD52114292 = -893424355;    float UCMYZtHqGsLvCLD71230820 = -767642446;    float UCMYZtHqGsLvCLD26263788 = 64311701;    float UCMYZtHqGsLvCLD56855407 = -809367236;    float UCMYZtHqGsLvCLD64756671 = -552540344;    float UCMYZtHqGsLvCLD74796850 = -130928981;    float UCMYZtHqGsLvCLD3523115 = 81756238;    float UCMYZtHqGsLvCLD41455281 = -550678921;    float UCMYZtHqGsLvCLD97260060 = -19222888;    float UCMYZtHqGsLvCLD24281272 = -601386867;    float UCMYZtHqGsLvCLD53701838 = -736457142;    float UCMYZtHqGsLvCLD38238875 = -836408248;    float UCMYZtHqGsLvCLD41321988 = -129135644;    float UCMYZtHqGsLvCLD30614602 = -611683413;    float UCMYZtHqGsLvCLD7065605 = -651158143;    float UCMYZtHqGsLvCLD57153264 = -123562880;    float UCMYZtHqGsLvCLD9975799 = -591583900;    float UCMYZtHqGsLvCLD36454680 = -225102247;    float UCMYZtHqGsLvCLD33158223 = -84339922;    float UCMYZtHqGsLvCLD10100305 = -567052982;    float UCMYZtHqGsLvCLD99763233 = -963459589;    float UCMYZtHqGsLvCLD18359224 = 32677111;    float UCMYZtHqGsLvCLD13769391 = 84187506;    float UCMYZtHqGsLvCLD83887081 = -502801668;    float UCMYZtHqGsLvCLD80074164 = -590156435;    float UCMYZtHqGsLvCLD68525440 = -334546904;    float UCMYZtHqGsLvCLD31453024 = -872865783;    float UCMYZtHqGsLvCLD13577985 = 16386393;    float UCMYZtHqGsLvCLD99662163 = -546045449;    float UCMYZtHqGsLvCLD82700398 = -448503946;    float UCMYZtHqGsLvCLD24787518 = -804321213;    float UCMYZtHqGsLvCLD78384424 = -826269385;    float UCMYZtHqGsLvCLD9333142 = -220064035;    float UCMYZtHqGsLvCLD58438296 = -803707518;    float UCMYZtHqGsLvCLD59584911 = -650298554;    float UCMYZtHqGsLvCLD63634264 = -522655873;    float UCMYZtHqGsLvCLD69517117 = -263966737;    float UCMYZtHqGsLvCLD37599693 = -609652437;    float UCMYZtHqGsLvCLD70143574 = -662633121;    float UCMYZtHqGsLvCLD67616726 = -424574816;    float UCMYZtHqGsLvCLD24954715 = -320271955;    float UCMYZtHqGsLvCLD61883685 = -657369946;    float UCMYZtHqGsLvCLD8562597 = -495491158;    float UCMYZtHqGsLvCLD71179938 = -471080037;    float UCMYZtHqGsLvCLD20679862 = -979414594;    float UCMYZtHqGsLvCLD38594428 = 4309424;    float UCMYZtHqGsLvCLD4007106 = -472319840;    float UCMYZtHqGsLvCLD67785300 = -719313643;    float UCMYZtHqGsLvCLD60761591 = -535806084;    float UCMYZtHqGsLvCLD28314297 = -509435214;    float UCMYZtHqGsLvCLD51382562 = 76689334;    float UCMYZtHqGsLvCLD64652582 = -453901509;    float UCMYZtHqGsLvCLD6388868 = -881627799;    float UCMYZtHqGsLvCLD74307145 = -644399006;    float UCMYZtHqGsLvCLD74117106 = -537553479;    float UCMYZtHqGsLvCLD33755069 = -826101467;    float UCMYZtHqGsLvCLD57461430 = -751829952;    float UCMYZtHqGsLvCLD42376707 = -432886631;    float UCMYZtHqGsLvCLD76781243 = -119210801;    float UCMYZtHqGsLvCLD96231230 = -117993441;    float UCMYZtHqGsLvCLD43343826 = -258063199;    float UCMYZtHqGsLvCLD89945130 = -934630155;    float UCMYZtHqGsLvCLD41793117 = 95366528;    float UCMYZtHqGsLvCLD14559662 = -570718943;    float UCMYZtHqGsLvCLD99493754 = -797065655;    float UCMYZtHqGsLvCLD75317413 = -910187757;    float UCMYZtHqGsLvCLD28905733 = -516344214;    float UCMYZtHqGsLvCLD82883691 = -325428127;    float UCMYZtHqGsLvCLD71029690 = -961384860;    float UCMYZtHqGsLvCLD43431340 = -28502270;    float UCMYZtHqGsLvCLD87636146 = -859596144;    float UCMYZtHqGsLvCLD72376105 = -981931464;    float UCMYZtHqGsLvCLD66311106 = -562469127;    float UCMYZtHqGsLvCLD65541496 = -659765107;    float UCMYZtHqGsLvCLD85145590 = -146781027;    float UCMYZtHqGsLvCLD37879548 = -892237105;     UCMYZtHqGsLvCLD17931515 = UCMYZtHqGsLvCLD93740592;     UCMYZtHqGsLvCLD93740592 = UCMYZtHqGsLvCLD32318636;     UCMYZtHqGsLvCLD32318636 = UCMYZtHqGsLvCLD11699203;     UCMYZtHqGsLvCLD11699203 = UCMYZtHqGsLvCLD34865085;     UCMYZtHqGsLvCLD34865085 = UCMYZtHqGsLvCLD95780904;     UCMYZtHqGsLvCLD95780904 = UCMYZtHqGsLvCLD94455100;     UCMYZtHqGsLvCLD94455100 = UCMYZtHqGsLvCLD34900245;     UCMYZtHqGsLvCLD34900245 = UCMYZtHqGsLvCLD42413577;     UCMYZtHqGsLvCLD42413577 = UCMYZtHqGsLvCLD28477830;     UCMYZtHqGsLvCLD28477830 = UCMYZtHqGsLvCLD3338966;     UCMYZtHqGsLvCLD3338966 = UCMYZtHqGsLvCLD5822658;     UCMYZtHqGsLvCLD5822658 = UCMYZtHqGsLvCLD95461209;     UCMYZtHqGsLvCLD95461209 = UCMYZtHqGsLvCLD74381699;     UCMYZtHqGsLvCLD74381699 = UCMYZtHqGsLvCLD76833303;     UCMYZtHqGsLvCLD76833303 = UCMYZtHqGsLvCLD45329093;     UCMYZtHqGsLvCLD45329093 = UCMYZtHqGsLvCLD98399901;     UCMYZtHqGsLvCLD98399901 = UCMYZtHqGsLvCLD67827196;     UCMYZtHqGsLvCLD67827196 = UCMYZtHqGsLvCLD85467560;     UCMYZtHqGsLvCLD85467560 = UCMYZtHqGsLvCLD61358361;     UCMYZtHqGsLvCLD61358361 = UCMYZtHqGsLvCLD1107263;     UCMYZtHqGsLvCLD1107263 = UCMYZtHqGsLvCLD39547090;     UCMYZtHqGsLvCLD39547090 = UCMYZtHqGsLvCLD84407450;     UCMYZtHqGsLvCLD84407450 = UCMYZtHqGsLvCLD73880339;     UCMYZtHqGsLvCLD73880339 = UCMYZtHqGsLvCLD52114292;     UCMYZtHqGsLvCLD52114292 = UCMYZtHqGsLvCLD71230820;     UCMYZtHqGsLvCLD71230820 = UCMYZtHqGsLvCLD26263788;     UCMYZtHqGsLvCLD26263788 = UCMYZtHqGsLvCLD56855407;     UCMYZtHqGsLvCLD56855407 = UCMYZtHqGsLvCLD64756671;     UCMYZtHqGsLvCLD64756671 = UCMYZtHqGsLvCLD74796850;     UCMYZtHqGsLvCLD74796850 = UCMYZtHqGsLvCLD3523115;     UCMYZtHqGsLvCLD3523115 = UCMYZtHqGsLvCLD41455281;     UCMYZtHqGsLvCLD41455281 = UCMYZtHqGsLvCLD97260060;     UCMYZtHqGsLvCLD97260060 = UCMYZtHqGsLvCLD24281272;     UCMYZtHqGsLvCLD24281272 = UCMYZtHqGsLvCLD53701838;     UCMYZtHqGsLvCLD53701838 = UCMYZtHqGsLvCLD38238875;     UCMYZtHqGsLvCLD38238875 = UCMYZtHqGsLvCLD41321988;     UCMYZtHqGsLvCLD41321988 = UCMYZtHqGsLvCLD30614602;     UCMYZtHqGsLvCLD30614602 = UCMYZtHqGsLvCLD7065605;     UCMYZtHqGsLvCLD7065605 = UCMYZtHqGsLvCLD57153264;     UCMYZtHqGsLvCLD57153264 = UCMYZtHqGsLvCLD9975799;     UCMYZtHqGsLvCLD9975799 = UCMYZtHqGsLvCLD36454680;     UCMYZtHqGsLvCLD36454680 = UCMYZtHqGsLvCLD33158223;     UCMYZtHqGsLvCLD33158223 = UCMYZtHqGsLvCLD10100305;     UCMYZtHqGsLvCLD10100305 = UCMYZtHqGsLvCLD99763233;     UCMYZtHqGsLvCLD99763233 = UCMYZtHqGsLvCLD18359224;     UCMYZtHqGsLvCLD18359224 = UCMYZtHqGsLvCLD13769391;     UCMYZtHqGsLvCLD13769391 = UCMYZtHqGsLvCLD83887081;     UCMYZtHqGsLvCLD83887081 = UCMYZtHqGsLvCLD80074164;     UCMYZtHqGsLvCLD80074164 = UCMYZtHqGsLvCLD68525440;     UCMYZtHqGsLvCLD68525440 = UCMYZtHqGsLvCLD31453024;     UCMYZtHqGsLvCLD31453024 = UCMYZtHqGsLvCLD13577985;     UCMYZtHqGsLvCLD13577985 = UCMYZtHqGsLvCLD99662163;     UCMYZtHqGsLvCLD99662163 = UCMYZtHqGsLvCLD82700398;     UCMYZtHqGsLvCLD82700398 = UCMYZtHqGsLvCLD24787518;     UCMYZtHqGsLvCLD24787518 = UCMYZtHqGsLvCLD78384424;     UCMYZtHqGsLvCLD78384424 = UCMYZtHqGsLvCLD9333142;     UCMYZtHqGsLvCLD9333142 = UCMYZtHqGsLvCLD58438296;     UCMYZtHqGsLvCLD58438296 = UCMYZtHqGsLvCLD59584911;     UCMYZtHqGsLvCLD59584911 = UCMYZtHqGsLvCLD63634264;     UCMYZtHqGsLvCLD63634264 = UCMYZtHqGsLvCLD69517117;     UCMYZtHqGsLvCLD69517117 = UCMYZtHqGsLvCLD37599693;     UCMYZtHqGsLvCLD37599693 = UCMYZtHqGsLvCLD70143574;     UCMYZtHqGsLvCLD70143574 = UCMYZtHqGsLvCLD67616726;     UCMYZtHqGsLvCLD67616726 = UCMYZtHqGsLvCLD24954715;     UCMYZtHqGsLvCLD24954715 = UCMYZtHqGsLvCLD61883685;     UCMYZtHqGsLvCLD61883685 = UCMYZtHqGsLvCLD8562597;     UCMYZtHqGsLvCLD8562597 = UCMYZtHqGsLvCLD71179938;     UCMYZtHqGsLvCLD71179938 = UCMYZtHqGsLvCLD20679862;     UCMYZtHqGsLvCLD20679862 = UCMYZtHqGsLvCLD38594428;     UCMYZtHqGsLvCLD38594428 = UCMYZtHqGsLvCLD4007106;     UCMYZtHqGsLvCLD4007106 = UCMYZtHqGsLvCLD67785300;     UCMYZtHqGsLvCLD67785300 = UCMYZtHqGsLvCLD60761591;     UCMYZtHqGsLvCLD60761591 = UCMYZtHqGsLvCLD28314297;     UCMYZtHqGsLvCLD28314297 = UCMYZtHqGsLvCLD51382562;     UCMYZtHqGsLvCLD51382562 = UCMYZtHqGsLvCLD64652582;     UCMYZtHqGsLvCLD64652582 = UCMYZtHqGsLvCLD6388868;     UCMYZtHqGsLvCLD6388868 = UCMYZtHqGsLvCLD74307145;     UCMYZtHqGsLvCLD74307145 = UCMYZtHqGsLvCLD74117106;     UCMYZtHqGsLvCLD74117106 = UCMYZtHqGsLvCLD33755069;     UCMYZtHqGsLvCLD33755069 = UCMYZtHqGsLvCLD57461430;     UCMYZtHqGsLvCLD57461430 = UCMYZtHqGsLvCLD42376707;     UCMYZtHqGsLvCLD42376707 = UCMYZtHqGsLvCLD76781243;     UCMYZtHqGsLvCLD76781243 = UCMYZtHqGsLvCLD96231230;     UCMYZtHqGsLvCLD96231230 = UCMYZtHqGsLvCLD43343826;     UCMYZtHqGsLvCLD43343826 = UCMYZtHqGsLvCLD89945130;     UCMYZtHqGsLvCLD89945130 = UCMYZtHqGsLvCLD41793117;     UCMYZtHqGsLvCLD41793117 = UCMYZtHqGsLvCLD14559662;     UCMYZtHqGsLvCLD14559662 = UCMYZtHqGsLvCLD99493754;     UCMYZtHqGsLvCLD99493754 = UCMYZtHqGsLvCLD75317413;     UCMYZtHqGsLvCLD75317413 = UCMYZtHqGsLvCLD28905733;     UCMYZtHqGsLvCLD28905733 = UCMYZtHqGsLvCLD82883691;     UCMYZtHqGsLvCLD82883691 = UCMYZtHqGsLvCLD71029690;     UCMYZtHqGsLvCLD71029690 = UCMYZtHqGsLvCLD43431340;     UCMYZtHqGsLvCLD43431340 = UCMYZtHqGsLvCLD87636146;     UCMYZtHqGsLvCLD87636146 = UCMYZtHqGsLvCLD72376105;     UCMYZtHqGsLvCLD72376105 = UCMYZtHqGsLvCLD66311106;     UCMYZtHqGsLvCLD66311106 = UCMYZtHqGsLvCLD65541496;     UCMYZtHqGsLvCLD65541496 = UCMYZtHqGsLvCLD85145590;     UCMYZtHqGsLvCLD85145590 = UCMYZtHqGsLvCLD37879548;     UCMYZtHqGsLvCLD37879548 = UCMYZtHqGsLvCLD17931515;}
// Junk Finished

// Junk Code By Peatreat & Thaisen's Gen
void BSedQwVFrBbWoYPr21425374() {     float negDXaOiJiPYEiJ14450907 = 81486348;    float negDXaOiJiPYEiJ70251101 = -441609198;    float negDXaOiJiPYEiJ86687734 = -95694738;    float negDXaOiJiPYEiJ39168190 = -91995869;    float negDXaOiJiPYEiJ94677372 = -477529993;    float negDXaOiJiPYEiJ98222344 = -630749800;    float negDXaOiJiPYEiJ99722417 = -879401180;    float negDXaOiJiPYEiJ1138365 = -623880332;    float negDXaOiJiPYEiJ63927669 = -94275480;    float negDXaOiJiPYEiJ85842852 = -360434058;    float negDXaOiJiPYEiJ43234476 = -223812032;    float negDXaOiJiPYEiJ6547418 = -347968589;    float negDXaOiJiPYEiJ68878994 = -336594092;    float negDXaOiJiPYEiJ87612592 = -880240219;    float negDXaOiJiPYEiJ36560951 = -820712682;    float negDXaOiJiPYEiJ83721369 = -127281049;    float negDXaOiJiPYEiJ70237990 = -424229531;    float negDXaOiJiPYEiJ80962151 = -407418669;    float negDXaOiJiPYEiJ46962267 = -604143127;    float negDXaOiJiPYEiJ30009857 = 87605056;    float negDXaOiJiPYEiJ29116694 = -179086981;    float negDXaOiJiPYEiJ2152582 = -933993530;    float negDXaOiJiPYEiJ37026017 = -33413855;    float negDXaOiJiPYEiJ54055001 = -579209950;    float negDXaOiJiPYEiJ1946610 = 7843738;    float negDXaOiJiPYEiJ30681108 = 61268220;    float negDXaOiJiPYEiJ3938891 = -562070433;    float negDXaOiJiPYEiJ13702245 = -956156347;    float negDXaOiJiPYEiJ34274195 = -871848012;    float negDXaOiJiPYEiJ98961487 = -920274449;    float negDXaOiJiPYEiJ79204515 = -933118681;    float negDXaOiJiPYEiJ92481607 = -490863180;    float negDXaOiJiPYEiJ29252077 = -807375440;    float negDXaOiJiPYEiJ36658103 = -545215134;    float negDXaOiJiPYEiJ80828638 = -691808743;    float negDXaOiJiPYEiJ39081939 = 66280790;    float negDXaOiJiPYEiJ9937587 = -639906224;    float negDXaOiJiPYEiJ33823896 = -437552703;    float negDXaOiJiPYEiJ20486315 = -279545477;    float negDXaOiJiPYEiJ64797770 = 49520924;    float negDXaOiJiPYEiJ54391126 = 64817828;    float negDXaOiJiPYEiJ94950371 = -538751128;    float negDXaOiJiPYEiJ60416799 = -21482240;    float negDXaOiJiPYEiJ63076515 = -51722292;    float negDXaOiJiPYEiJ81731895 = 77645475;    float negDXaOiJiPYEiJ75612282 = -487606015;    float negDXaOiJiPYEiJ82983690 = -813052520;    float negDXaOiJiPYEiJ88993523 = -57134311;    float negDXaOiJiPYEiJ80024842 = -557896769;    float negDXaOiJiPYEiJ21281056 = 2938527;    float negDXaOiJiPYEiJ69982294 = -823689982;    float negDXaOiJiPYEiJ71439366 = -183633109;    float negDXaOiJiPYEiJ16357194 = -345900085;    float negDXaOiJiPYEiJ57957324 = -366785099;    float negDXaOiJiPYEiJ92308680 = -380143728;    float negDXaOiJiPYEiJ12298326 = 15479877;    float negDXaOiJiPYEiJ33225085 = -308195343;    float negDXaOiJiPYEiJ32632734 = -516484788;    float negDXaOiJiPYEiJ37221581 = 160392;    float negDXaOiJiPYEiJ63996265 = -438798213;    float negDXaOiJiPYEiJ94283453 = 31320633;    float negDXaOiJiPYEiJ86020172 = -923244834;    float negDXaOiJiPYEiJ66864169 = -752032320;    float negDXaOiJiPYEiJ64966182 = -174001031;    float negDXaOiJiPYEiJ6638337 = -427315378;    float negDXaOiJiPYEiJ50752869 = -732948852;    float negDXaOiJiPYEiJ77295341 = -540593150;    float negDXaOiJiPYEiJ32220891 = -791378959;    float negDXaOiJiPYEiJ6783954 = -88431477;    float negDXaOiJiPYEiJ97479012 = -786993472;    float negDXaOiJiPYEiJ73783782 = -487374825;    float negDXaOiJiPYEiJ36414095 = -986676829;    float negDXaOiJiPYEiJ60475837 = -27873193;    float negDXaOiJiPYEiJ82164497 = -553664052;    float negDXaOiJiPYEiJ75618731 = -977212772;    float negDXaOiJiPYEiJ34166322 = -640335854;    float negDXaOiJiPYEiJ41735783 = -812511290;    float negDXaOiJiPYEiJ73949501 = -981691564;    float negDXaOiJiPYEiJ72323105 = -556855425;    float negDXaOiJiPYEiJ26334328 = -504550247;    float negDXaOiJiPYEiJ47697418 = -125679260;    float negDXaOiJiPYEiJ14945368 = -404936122;    float negDXaOiJiPYEiJ33677403 = -298259578;    float negDXaOiJiPYEiJ12993140 = -774786540;    float negDXaOiJiPYEiJ28979193 = 3415532;    float negDXaOiJiPYEiJ7765150 = -649485572;    float negDXaOiJiPYEiJ76124413 = -44963095;    float negDXaOiJiPYEiJ71294753 = -340590342;    float negDXaOiJiPYEiJ44349423 = -65071407;    float negDXaOiJiPYEiJ68530313 = -607288620;    float negDXaOiJiPYEiJ5856855 = -625523867;    float negDXaOiJiPYEiJ77304853 = -23421436;    float negDXaOiJiPYEiJ96602315 = -337713095;    float negDXaOiJiPYEiJ56490050 = -840747264;    float negDXaOiJiPYEiJ70514316 = -981799709;    float negDXaOiJiPYEiJ68370953 = -11937338;    float negDXaOiJiPYEiJ28086202 = -786718808;    float negDXaOiJiPYEiJ95450617 = -847481209;    float negDXaOiJiPYEiJ56438178 = -624406914;    float negDXaOiJiPYEiJ30979027 = 81486348;     negDXaOiJiPYEiJ14450907 = negDXaOiJiPYEiJ70251101;     negDXaOiJiPYEiJ70251101 = negDXaOiJiPYEiJ86687734;     negDXaOiJiPYEiJ86687734 = negDXaOiJiPYEiJ39168190;     negDXaOiJiPYEiJ39168190 = negDXaOiJiPYEiJ94677372;     negDXaOiJiPYEiJ94677372 = negDXaOiJiPYEiJ98222344;     negDXaOiJiPYEiJ98222344 = negDXaOiJiPYEiJ99722417;     negDXaOiJiPYEiJ99722417 = negDXaOiJiPYEiJ1138365;     negDXaOiJiPYEiJ1138365 = negDXaOiJiPYEiJ63927669;     negDXaOiJiPYEiJ63927669 = negDXaOiJiPYEiJ85842852;     negDXaOiJiPYEiJ85842852 = negDXaOiJiPYEiJ43234476;     negDXaOiJiPYEiJ43234476 = negDXaOiJiPYEiJ6547418;     negDXaOiJiPYEiJ6547418 = negDXaOiJiPYEiJ68878994;     negDXaOiJiPYEiJ68878994 = negDXaOiJiPYEiJ87612592;     negDXaOiJiPYEiJ87612592 = negDXaOiJiPYEiJ36560951;     negDXaOiJiPYEiJ36560951 = negDXaOiJiPYEiJ83721369;     negDXaOiJiPYEiJ83721369 = negDXaOiJiPYEiJ70237990;     negDXaOiJiPYEiJ70237990 = negDXaOiJiPYEiJ80962151;     negDXaOiJiPYEiJ80962151 = negDXaOiJiPYEiJ46962267;     negDXaOiJiPYEiJ46962267 = negDXaOiJiPYEiJ30009857;     negDXaOiJiPYEiJ30009857 = negDXaOiJiPYEiJ29116694;     negDXaOiJiPYEiJ29116694 = negDXaOiJiPYEiJ2152582;     negDXaOiJiPYEiJ2152582 = negDXaOiJiPYEiJ37026017;     negDXaOiJiPYEiJ37026017 = negDXaOiJiPYEiJ54055001;     negDXaOiJiPYEiJ54055001 = negDXaOiJiPYEiJ1946610;     negDXaOiJiPYEiJ1946610 = negDXaOiJiPYEiJ30681108;     negDXaOiJiPYEiJ30681108 = negDXaOiJiPYEiJ3938891;     negDXaOiJiPYEiJ3938891 = negDXaOiJiPYEiJ13702245;     negDXaOiJiPYEiJ13702245 = negDXaOiJiPYEiJ34274195;     negDXaOiJiPYEiJ34274195 = negDXaOiJiPYEiJ98961487;     negDXaOiJiPYEiJ98961487 = negDXaOiJiPYEiJ79204515;     negDXaOiJiPYEiJ79204515 = negDXaOiJiPYEiJ92481607;     negDXaOiJiPYEiJ92481607 = negDXaOiJiPYEiJ29252077;     negDXaOiJiPYEiJ29252077 = negDXaOiJiPYEiJ36658103;     negDXaOiJiPYEiJ36658103 = negDXaOiJiPYEiJ80828638;     negDXaOiJiPYEiJ80828638 = negDXaOiJiPYEiJ39081939;     negDXaOiJiPYEiJ39081939 = negDXaOiJiPYEiJ9937587;     negDXaOiJiPYEiJ9937587 = negDXaOiJiPYEiJ33823896;     negDXaOiJiPYEiJ33823896 = negDXaOiJiPYEiJ20486315;     negDXaOiJiPYEiJ20486315 = negDXaOiJiPYEiJ64797770;     negDXaOiJiPYEiJ64797770 = negDXaOiJiPYEiJ54391126;     negDXaOiJiPYEiJ54391126 = negDXaOiJiPYEiJ94950371;     negDXaOiJiPYEiJ94950371 = negDXaOiJiPYEiJ60416799;     negDXaOiJiPYEiJ60416799 = negDXaOiJiPYEiJ63076515;     negDXaOiJiPYEiJ63076515 = negDXaOiJiPYEiJ81731895;     negDXaOiJiPYEiJ81731895 = negDXaOiJiPYEiJ75612282;     negDXaOiJiPYEiJ75612282 = negDXaOiJiPYEiJ82983690;     negDXaOiJiPYEiJ82983690 = negDXaOiJiPYEiJ88993523;     negDXaOiJiPYEiJ88993523 = negDXaOiJiPYEiJ80024842;     negDXaOiJiPYEiJ80024842 = negDXaOiJiPYEiJ21281056;     negDXaOiJiPYEiJ21281056 = negDXaOiJiPYEiJ69982294;     negDXaOiJiPYEiJ69982294 = negDXaOiJiPYEiJ71439366;     negDXaOiJiPYEiJ71439366 = negDXaOiJiPYEiJ16357194;     negDXaOiJiPYEiJ16357194 = negDXaOiJiPYEiJ57957324;     negDXaOiJiPYEiJ57957324 = negDXaOiJiPYEiJ92308680;     negDXaOiJiPYEiJ92308680 = negDXaOiJiPYEiJ12298326;     negDXaOiJiPYEiJ12298326 = negDXaOiJiPYEiJ33225085;     negDXaOiJiPYEiJ33225085 = negDXaOiJiPYEiJ32632734;     negDXaOiJiPYEiJ32632734 = negDXaOiJiPYEiJ37221581;     negDXaOiJiPYEiJ37221581 = negDXaOiJiPYEiJ63996265;     negDXaOiJiPYEiJ63996265 = negDXaOiJiPYEiJ94283453;     negDXaOiJiPYEiJ94283453 = negDXaOiJiPYEiJ86020172;     negDXaOiJiPYEiJ86020172 = negDXaOiJiPYEiJ66864169;     negDXaOiJiPYEiJ66864169 = negDXaOiJiPYEiJ64966182;     negDXaOiJiPYEiJ64966182 = negDXaOiJiPYEiJ6638337;     negDXaOiJiPYEiJ6638337 = negDXaOiJiPYEiJ50752869;     negDXaOiJiPYEiJ50752869 = negDXaOiJiPYEiJ77295341;     negDXaOiJiPYEiJ77295341 = negDXaOiJiPYEiJ32220891;     negDXaOiJiPYEiJ32220891 = negDXaOiJiPYEiJ6783954;     negDXaOiJiPYEiJ6783954 = negDXaOiJiPYEiJ97479012;     negDXaOiJiPYEiJ97479012 = negDXaOiJiPYEiJ73783782;     negDXaOiJiPYEiJ73783782 = negDXaOiJiPYEiJ36414095;     negDXaOiJiPYEiJ36414095 = negDXaOiJiPYEiJ60475837;     negDXaOiJiPYEiJ60475837 = negDXaOiJiPYEiJ82164497;     negDXaOiJiPYEiJ82164497 = negDXaOiJiPYEiJ75618731;     negDXaOiJiPYEiJ75618731 = negDXaOiJiPYEiJ34166322;     negDXaOiJiPYEiJ34166322 = negDXaOiJiPYEiJ41735783;     negDXaOiJiPYEiJ41735783 = negDXaOiJiPYEiJ73949501;     negDXaOiJiPYEiJ73949501 = negDXaOiJiPYEiJ72323105;     negDXaOiJiPYEiJ72323105 = negDXaOiJiPYEiJ26334328;     negDXaOiJiPYEiJ26334328 = negDXaOiJiPYEiJ47697418;     negDXaOiJiPYEiJ47697418 = negDXaOiJiPYEiJ14945368;     negDXaOiJiPYEiJ14945368 = negDXaOiJiPYEiJ33677403;     negDXaOiJiPYEiJ33677403 = negDXaOiJiPYEiJ12993140;     negDXaOiJiPYEiJ12993140 = negDXaOiJiPYEiJ28979193;     negDXaOiJiPYEiJ28979193 = negDXaOiJiPYEiJ7765150;     negDXaOiJiPYEiJ7765150 = negDXaOiJiPYEiJ76124413;     negDXaOiJiPYEiJ76124413 = negDXaOiJiPYEiJ71294753;     negDXaOiJiPYEiJ71294753 = negDXaOiJiPYEiJ44349423;     negDXaOiJiPYEiJ44349423 = negDXaOiJiPYEiJ68530313;     negDXaOiJiPYEiJ68530313 = negDXaOiJiPYEiJ5856855;     negDXaOiJiPYEiJ5856855 = negDXaOiJiPYEiJ77304853;     negDXaOiJiPYEiJ77304853 = negDXaOiJiPYEiJ96602315;     negDXaOiJiPYEiJ96602315 = negDXaOiJiPYEiJ56490050;     negDXaOiJiPYEiJ56490050 = negDXaOiJiPYEiJ70514316;     negDXaOiJiPYEiJ70514316 = negDXaOiJiPYEiJ68370953;     negDXaOiJiPYEiJ68370953 = negDXaOiJiPYEiJ28086202;     negDXaOiJiPYEiJ28086202 = negDXaOiJiPYEiJ95450617;     negDXaOiJiPYEiJ95450617 = negDXaOiJiPYEiJ56438178;     negDXaOiJiPYEiJ56438178 = negDXaOiJiPYEiJ30979027;     negDXaOiJiPYEiJ30979027 = negDXaOiJiPYEiJ14450907;}
// Junk Finished

// Junk Code By Peatreat & Thaisen's Gen
void evbaFLxVYFkEXPGv67121066() {     float IIYMWVvnlGATDxv33639865 = -862288478;    float IIYMWVvnlGATDxv42548959 = -83217531;    float IIYMWVvnlGATDxv11347117 = -772540890;    float IIYMWVvnlGATDxv23459638 = -97488039;    float IIYMWVvnlGATDxv21683877 = -512721708;    float IIYMWVvnlGATDxv10979302 = -196985552;    float IIYMWVvnlGATDxv78713051 = 58895415;    float IIYMWVvnlGATDxv37395958 = -84391742;    float IIYMWVvnlGATDxv32924639 = -466443513;    float IIYMWVvnlGATDxv50211639 = -611355201;    float IIYMWVvnlGATDxv3984642 = 93367053;    float IIYMWVvnlGATDxv83035659 = -405750269;    float IIYMWVvnlGATDxv26314577 = -709780366;    float IIYMWVvnlGATDxv81282380 = -861133439;    float IIYMWVvnlGATDxv53762617 = -187122150;    float IIYMWVvnlGATDxv64512151 = -454424657;    float IIYMWVvnlGATDxv70086578 = -190469377;    float IIYMWVvnlGATDxv71799079 = -794626690;    float IIYMWVvnlGATDxv68060808 = -91373520;    float IIYMWVvnlGATDxv25880957 = -443224476;    float IIYMWVvnlGATDxv29873643 = -800361817;    float IIYMWVvnlGATDxv28279912 = -423793227;    float IIYMWVvnlGATDxv39848865 = -341482934;    float IIYMWVvnlGATDxv5340533 = -748346685;    float IIYMWVvnlGATDxv18123964 = -767778368;    float IIYMWVvnlGATDxv77797960 = -162069686;    float IIYMWVvnlGATDxv15729823 = -511430284;    float IIYMWVvnlGATDxv61626345 = -303403021;    float IIYMWVvnlGATDxv40182105 = -224068400;    float IIYMWVvnlGATDxv63953356 = -851956621;    float IIYMWVvnlGATDxv45739492 = -217103954;    float IIYMWVvnlGATDxv70374056 = -421210055;    float IIYMWVvnlGATDxv70142365 = -318241293;    float IIYMWVvnlGATDxv1848407 = -37323863;    float IIYMWVvnlGATDxv31882380 = -117885676;    float IIYMWVvnlGATDxv17675827 = -46915247;    float IIYMWVvnlGATDxv47171626 = -725290182;    float IIYMWVvnlGATDxv65503411 = -324088822;    float IIYMWVvnlGATDxv76178998 = -623299218;    float IIYMWVvnlGATDxv96385525 = -852956815;    float IIYMWVvnlGATDxv64754587 = -366477314;    float IIYMWVvnlGATDxv27957564 = -645530006;    float IIYMWVvnlGATDxv22316964 = -867937026;    float IIYMWVvnlGATDxv91171474 = -678033991;    float IIYMWVvnlGATDxv25717553 = -701620363;    float IIYMWVvnlGATDxv25412524 = -329516668;    float IIYMWVvnlGATDxv7810032 = -837404166;    float IIYMWVvnlGATDxv70078101 = -477021446;    float IIYMWVvnlGATDxv18133333 = -357653939;    float IIYMWVvnlGATDxv61638552 = -433078838;    float IIYMWVvnlGATDxv77584480 = -185183933;    float IIYMWVvnlGATDxv91883300 = -867192494;    float IIYMWVvnlGATDxv54912397 = -221823996;    float IIYMWVvnlGATDxv9631506 = -603082935;    float IIYMWVvnlGATDxv37247424 = -186461541;    float IIYMWVvnlGATDxv5359954 = -338495251;    float IIYMWVvnlGATDxv2700094 = -741734597;    float IIYMWVvnlGATDxv6006584 = 75805794;    float IIYMWVvnlGATDxv5335674 = -329709671;    float IIYMWVvnlGATDxv43885916 = -250652022;    float IIYMWVvnlGATDxv95249479 = -685555269;    float IIYMWVvnlGATDxv17086706 = -637701565;    float IIYMWVvnlGATDxv97213853 = -860323342;    float IIYMWVvnlGATDxv68971283 = -614486893;    float IIYMWVvnlGATDxv4472148 = -294251247;    float IIYMWVvnlGATDxv33610585 = -485422892;    float IIYMWVvnlGATDxv12893294 = 12491023;    float IIYMWVvnlGATDxv24466171 = -572456504;    float IIYMWVvnlGATDxv49400001 = -643247764;    float IIYMWVvnlGATDxv36086791 = -40206904;    float IIYMWVvnlGATDxv17340526 = -729134475;    float IIYMWVvnlGATDxv4583167 = -866380555;    float IIYMWVvnlGATDxv95620081 = -71327472;    float IIYMWVvnlGATDxv71675282 = -238416705;    float IIYMWVvnlGATDxv61126370 = 23252838;    float IIYMWVvnlGATDxv1916079 = -54831811;    float IIYMWVvnlGATDxv5962949 = -555856201;    float IIYMWVvnlGATDxv48677391 = -663448944;    float IIYMWVvnlGATDxv79622980 = 53273678;    float IIYMWVvnlGATDxv92711440 = -338261701;    float IIYMWVvnlGATDxv69987929 = -324665521;    float IIYMWVvnlGATDxv45651721 = 65591162;    float IIYMWVvnlGATDxv43493013 = -945749083;    float IIYMWVvnlGATDxv78543552 = -790989563;    float IIYMWVvnlGATDxv86368875 = -566772688;    float IIYMWVvnlGATDxv53856191 = -349911461;    float IIYMWVvnlGATDxv15461660 = -99386059;    float IIYMWVvnlGATDxv60510860 = -715158359;    float IIYMWVvnlGATDxv64600982 = -850862323;    float IIYMWVvnlGATDxv26522427 = -779390425;    float IIYMWVvnlGATDxv14975733 = -305180650;    float IIYMWVvnlGATDxv41165043 = -701095977;    float IIYMWVvnlGATDxv60167738 = -994379151;    float IIYMWVvnlGATDxv32293083 = -272647197;    float IIYMWVvnlGATDxv1136047 = -67401547;    float IIYMWVvnlGATDxv47667881 = -728775750;    float IIYMWVvnlGATDxv30743711 = -785206664;    float IIYMWVvnlGATDxv53345680 = -153450133;    float IIYMWVvnlGATDxv86699326 = -283782744;    float IIYMWVvnlGATDxv92106967 = -862288478;     IIYMWVvnlGATDxv33639865 = IIYMWVvnlGATDxv42548959;     IIYMWVvnlGATDxv42548959 = IIYMWVvnlGATDxv11347117;     IIYMWVvnlGATDxv11347117 = IIYMWVvnlGATDxv23459638;     IIYMWVvnlGATDxv23459638 = IIYMWVvnlGATDxv21683877;     IIYMWVvnlGATDxv21683877 = IIYMWVvnlGATDxv10979302;     IIYMWVvnlGATDxv10979302 = IIYMWVvnlGATDxv78713051;     IIYMWVvnlGATDxv78713051 = IIYMWVvnlGATDxv37395958;     IIYMWVvnlGATDxv37395958 = IIYMWVvnlGATDxv32924639;     IIYMWVvnlGATDxv32924639 = IIYMWVvnlGATDxv50211639;     IIYMWVvnlGATDxv50211639 = IIYMWVvnlGATDxv3984642;     IIYMWVvnlGATDxv3984642 = IIYMWVvnlGATDxv83035659;     IIYMWVvnlGATDxv83035659 = IIYMWVvnlGATDxv26314577;     IIYMWVvnlGATDxv26314577 = IIYMWVvnlGATDxv81282380;     IIYMWVvnlGATDxv81282380 = IIYMWVvnlGATDxv53762617;     IIYMWVvnlGATDxv53762617 = IIYMWVvnlGATDxv64512151;     IIYMWVvnlGATDxv64512151 = IIYMWVvnlGATDxv70086578;     IIYMWVvnlGATDxv70086578 = IIYMWVvnlGATDxv71799079;     IIYMWVvnlGATDxv71799079 = IIYMWVvnlGATDxv68060808;     IIYMWVvnlGATDxv68060808 = IIYMWVvnlGATDxv25880957;     IIYMWVvnlGATDxv25880957 = IIYMWVvnlGATDxv29873643;     IIYMWVvnlGATDxv29873643 = IIYMWVvnlGATDxv28279912;     IIYMWVvnlGATDxv28279912 = IIYMWVvnlGATDxv39848865;     IIYMWVvnlGATDxv39848865 = IIYMWVvnlGATDxv5340533;     IIYMWVvnlGATDxv5340533 = IIYMWVvnlGATDxv18123964;     IIYMWVvnlGATDxv18123964 = IIYMWVvnlGATDxv77797960;     IIYMWVvnlGATDxv77797960 = IIYMWVvnlGATDxv15729823;     IIYMWVvnlGATDxv15729823 = IIYMWVvnlGATDxv61626345;     IIYMWVvnlGATDxv61626345 = IIYMWVvnlGATDxv40182105;     IIYMWVvnlGATDxv40182105 = IIYMWVvnlGATDxv63953356;     IIYMWVvnlGATDxv63953356 = IIYMWVvnlGATDxv45739492;     IIYMWVvnlGATDxv45739492 = IIYMWVvnlGATDxv70374056;     IIYMWVvnlGATDxv70374056 = IIYMWVvnlGATDxv70142365;     IIYMWVvnlGATDxv70142365 = IIYMWVvnlGATDxv1848407;     IIYMWVvnlGATDxv1848407 = IIYMWVvnlGATDxv31882380;     IIYMWVvnlGATDxv31882380 = IIYMWVvnlGATDxv17675827;     IIYMWVvnlGATDxv17675827 = IIYMWVvnlGATDxv47171626;     IIYMWVvnlGATDxv47171626 = IIYMWVvnlGATDxv65503411;     IIYMWVvnlGATDxv65503411 = IIYMWVvnlGATDxv76178998;     IIYMWVvnlGATDxv76178998 = IIYMWVvnlGATDxv96385525;     IIYMWVvnlGATDxv96385525 = IIYMWVvnlGATDxv64754587;     IIYMWVvnlGATDxv64754587 = IIYMWVvnlGATDxv27957564;     IIYMWVvnlGATDxv27957564 = IIYMWVvnlGATDxv22316964;     IIYMWVvnlGATDxv22316964 = IIYMWVvnlGATDxv91171474;     IIYMWVvnlGATDxv91171474 = IIYMWVvnlGATDxv25717553;     IIYMWVvnlGATDxv25717553 = IIYMWVvnlGATDxv25412524;     IIYMWVvnlGATDxv25412524 = IIYMWVvnlGATDxv7810032;     IIYMWVvnlGATDxv7810032 = IIYMWVvnlGATDxv70078101;     IIYMWVvnlGATDxv70078101 = IIYMWVvnlGATDxv18133333;     IIYMWVvnlGATDxv18133333 = IIYMWVvnlGATDxv61638552;     IIYMWVvnlGATDxv61638552 = IIYMWVvnlGATDxv77584480;     IIYMWVvnlGATDxv77584480 = IIYMWVvnlGATDxv91883300;     IIYMWVvnlGATDxv91883300 = IIYMWVvnlGATDxv54912397;     IIYMWVvnlGATDxv54912397 = IIYMWVvnlGATDxv9631506;     IIYMWVvnlGATDxv9631506 = IIYMWVvnlGATDxv37247424;     IIYMWVvnlGATDxv37247424 = IIYMWVvnlGATDxv5359954;     IIYMWVvnlGATDxv5359954 = IIYMWVvnlGATDxv2700094;     IIYMWVvnlGATDxv2700094 = IIYMWVvnlGATDxv6006584;     IIYMWVvnlGATDxv6006584 = IIYMWVvnlGATDxv5335674;     IIYMWVvnlGATDxv5335674 = IIYMWVvnlGATDxv43885916;     IIYMWVvnlGATDxv43885916 = IIYMWVvnlGATDxv95249479;     IIYMWVvnlGATDxv95249479 = IIYMWVvnlGATDxv17086706;     IIYMWVvnlGATDxv17086706 = IIYMWVvnlGATDxv97213853;     IIYMWVvnlGATDxv97213853 = IIYMWVvnlGATDxv68971283;     IIYMWVvnlGATDxv68971283 = IIYMWVvnlGATDxv4472148;     IIYMWVvnlGATDxv4472148 = IIYMWVvnlGATDxv33610585;     IIYMWVvnlGATDxv33610585 = IIYMWVvnlGATDxv12893294;     IIYMWVvnlGATDxv12893294 = IIYMWVvnlGATDxv24466171;     IIYMWVvnlGATDxv24466171 = IIYMWVvnlGATDxv49400001;     IIYMWVvnlGATDxv49400001 = IIYMWVvnlGATDxv36086791;     IIYMWVvnlGATDxv36086791 = IIYMWVvnlGATDxv17340526;     IIYMWVvnlGATDxv17340526 = IIYMWVvnlGATDxv4583167;     IIYMWVvnlGATDxv4583167 = IIYMWVvnlGATDxv95620081;     IIYMWVvnlGATDxv95620081 = IIYMWVvnlGATDxv71675282;     IIYMWVvnlGATDxv71675282 = IIYMWVvnlGATDxv61126370;     IIYMWVvnlGATDxv61126370 = IIYMWVvnlGATDxv1916079;     IIYMWVvnlGATDxv1916079 = IIYMWVvnlGATDxv5962949;     IIYMWVvnlGATDxv5962949 = IIYMWVvnlGATDxv48677391;     IIYMWVvnlGATDxv48677391 = IIYMWVvnlGATDxv79622980;     IIYMWVvnlGATDxv79622980 = IIYMWVvnlGATDxv92711440;     IIYMWVvnlGATDxv92711440 = IIYMWVvnlGATDxv69987929;     IIYMWVvnlGATDxv69987929 = IIYMWVvnlGATDxv45651721;     IIYMWVvnlGATDxv45651721 = IIYMWVvnlGATDxv43493013;     IIYMWVvnlGATDxv43493013 = IIYMWVvnlGATDxv78543552;     IIYMWVvnlGATDxv78543552 = IIYMWVvnlGATDxv86368875;     IIYMWVvnlGATDxv86368875 = IIYMWVvnlGATDxv53856191;     IIYMWVvnlGATDxv53856191 = IIYMWVvnlGATDxv15461660;     IIYMWVvnlGATDxv15461660 = IIYMWVvnlGATDxv60510860;     IIYMWVvnlGATDxv60510860 = IIYMWVvnlGATDxv64600982;     IIYMWVvnlGATDxv64600982 = IIYMWVvnlGATDxv26522427;     IIYMWVvnlGATDxv26522427 = IIYMWVvnlGATDxv14975733;     IIYMWVvnlGATDxv14975733 = IIYMWVvnlGATDxv41165043;     IIYMWVvnlGATDxv41165043 = IIYMWVvnlGATDxv60167738;     IIYMWVvnlGATDxv60167738 = IIYMWVvnlGATDxv32293083;     IIYMWVvnlGATDxv32293083 = IIYMWVvnlGATDxv1136047;     IIYMWVvnlGATDxv1136047 = IIYMWVvnlGATDxv47667881;     IIYMWVvnlGATDxv47667881 = IIYMWVvnlGATDxv30743711;     IIYMWVvnlGATDxv30743711 = IIYMWVvnlGATDxv53345680;     IIYMWVvnlGATDxv53345680 = IIYMWVvnlGATDxv86699326;     IIYMWVvnlGATDxv86699326 = IIYMWVvnlGATDxv92106967;     IIYMWVvnlGATDxv92106967 = IIYMWVvnlGATDxv33639865;}
// Junk Finished

// Junk Code By Peatreat & Thaisen's Gen
void iTPBUgyjiPTzsoTl21537280() {     float fnDmZBtVmmnBWcK4291532 = -380437606;    float fnDmZBtVmmnBWcK11222477 = 45139176;    float fnDmZBtVmmnBWcK91316035 = -114835350;    float fnDmZBtVmmnBWcK63390924 = -287626431;    float fnDmZBtVmmnBWcK43134198 = -296128047;    float fnDmZBtVmmnBWcK979065 = -998566704;    float fnDmZBtVmmnBWcK97091946 = 60119705;    float fnDmZBtVmmnBWcK94238338 = -431208816;    float fnDmZBtVmmnBWcK78789693 = -559790841;    float fnDmZBtVmmnBWcK73178100 = -912450552;    float fnDmZBtVmmnBWcK46233802 = 58465125;    float fnDmZBtVmmnBWcK28582231 = -721514053;    float fnDmZBtVmmnBWcK6141965 = -619317425;    float fnDmZBtVmmnBWcK65129414 = -662161648;    float fnDmZBtVmmnBWcK36482637 = -336872135;    float fnDmZBtVmmnBWcK97040791 = -24408243;    float fnDmZBtVmmnBWcK63939405 = -99654507;    float fnDmZBtVmmnBWcK92092573 = -783792882;    float fnDmZBtVmmnBWcK49768589 = -798561463;    float fnDmZBtVmmnBWcK84667666 = -985590781;    float fnDmZBtVmmnBWcK85009396 = -709040952;    float fnDmZBtVmmnBWcK89675676 = -755078533;    float fnDmZBtVmmnBWcK41792673 = -909326079;    float fnDmZBtVmmnBWcK39190070 = -340747591;    float fnDmZBtVmmnBWcK82248352 = -443683044;    float fnDmZBtVmmnBWcK8395541 = -260354135;    float fnDmZBtVmmnBWcK23990608 = -906608059;    float fnDmZBtVmmnBWcK69408570 = -808526782;    float fnDmZBtVmmnBWcK72324592 = -518426633;    float fnDmZBtVmmnBWcK3125452 = -310351691;    float fnDmZBtVmmnBWcK52811488 = 34462360;    float fnDmZBtVmmnBWcK14885517 = -749507221;    float fnDmZBtVmmnBWcK53087726 = -265202905;    float fnDmZBtVmmnBWcK67213381 = -700853547;    float fnDmZBtVmmnBWcK9906616 = 35167541;    float fnDmZBtVmmnBWcK77669085 = -126141813;    float fnDmZBtVmmnBWcK90963111 = -913883390;    float fnDmZBtVmmnBWcK18295900 = -108201926;    float fnDmZBtVmmnBWcK54712287 = -579027867;    float fnDmZBtVmmnBWcK91488824 = -370565178;    float fnDmZBtVmmnBWcK24827871 = -750496193;    float fnDmZBtVmmnBWcK56440555 = -414919096;    float fnDmZBtVmmnBWcK71283923 = -854691080;    float fnDmZBtVmmnBWcK97679774 = -750008963;    float fnDmZBtVmmnBWcK21671612 = 89733241;    float fnDmZBtVmmnBWcK31818359 = -87853841;    float fnDmZBtVmmnBWcK33941235 = -953473965;    float fnDmZBtVmmnBWcK35692312 = -929151742;    float fnDmZBtVmmnBWcK23574785 = -343417933;    float fnDmZBtVmmnBWcK70066039 = -716087712;    float fnDmZBtVmmnBWcK45773804 = -831756055;    float fnDmZBtVmmnBWcK33754805 = -836412220;    float fnDmZBtVmmnBWcK9988055 = -149622035;    float fnDmZBtVmmnBWcK99858809 = -163964431;    float fnDmZBtVmmnBWcK36527883 = -297402460;    float fnDmZBtVmmnBWcK14615856 = -625359074;    float fnDmZBtVmmnBWcK69429803 = -45534746;    float fnDmZBtVmmnBWcK52125965 = -774087759;    float fnDmZBtVmmnBWcK81142571 = -843943388;    float fnDmZBtVmmnBWcK34738657 = 64226087;    float fnDmZBtVmmnBWcK76988456 = 8041355;    float fnDmZBtVmmnBWcK27683376 = -131353513;    float fnDmZBtVmmnBWcK21913746 = -912782184;    float fnDmZBtVmmnBWcK75664241 = -149439150;    float fnDmZBtVmmnBWcK20366612 = -846912912;    float fnDmZBtVmmnBWcK31348285 = -192027654;    float fnDmZBtVmmnBWcK75494504 = -356311149;    float fnDmZBtVmmnBWcK38928583 = -918463878;    float fnDmZBtVmmnBWcK55222798 = -597329190;    float fnDmZBtVmmnBWcK58813551 = -110730322;    float fnDmZBtVmmnBWcK6077680 = -110524854;    float fnDmZBtVmmnBWcK45643505 = -991452581;    float fnDmZBtVmmnBWcK37380287 = -104765015;    float fnDmZBtVmmnBWcK58279765 = -327996286;    float fnDmZBtVmmnBWcK59839795 = -135094588;    float fnDmZBtVmmnBWcK28568842 = -194121857;    float fnDmZBtVmmnBWcK18391753 = -900387453;    float fnDmZBtVmmnBWcK44112899 = -59317117;    float fnDmZBtVmmnBWcK17518459 = -330480832;    float fnDmZBtVmmnBWcK50429994 = -255829203;    float fnDmZBtVmmnBWcK74454306 = -306880171;    float fnDmZBtVmmnBWcK88298296 = -977456317;    float fnDmZBtVmmnBWcK45833786 = -365108850;    float fnDmZBtVmmnBWcK2258554 = -802338922;    float fnDmZBtVmmnBWcK57351647 = -478595637;    float fnDmZBtVmmnBWcK19056683 = -129125420;    float fnDmZBtVmmnBWcK4897463 = -499885186;    float fnDmZBtVmmnBWcK53228916 = -1238474;    float fnDmZBtVmmnBWcK30685499 = -303451087;    float fnDmZBtVmmnBWcK95290760 = -339473385;    float fnDmZBtVmmnBWcK8239283 = 19392932;    float fnDmZBtVmmnBWcK38837147 = -39795631;    float fnDmZBtVmmnBWcK37153328 = -264258538;    float fnDmZBtVmmnBWcK19973631 = -543253955;    float fnDmZBtVmmnBWcK14500368 = -278606533;    float fnDmZBtVmmnBWcK97144494 = -519142680;    float fnDmZBtVmmnBWcK34526809 = -502136912;    float fnDmZBtVmmnBWcK95619682 = -605251930;    float fnDmZBtVmmnBWcK77313162 = -903096051;    float fnDmZBtVmmnBWcK90323326 = -380437606;     fnDmZBtVmmnBWcK4291532 = fnDmZBtVmmnBWcK11222477;     fnDmZBtVmmnBWcK11222477 = fnDmZBtVmmnBWcK91316035;     fnDmZBtVmmnBWcK91316035 = fnDmZBtVmmnBWcK63390924;     fnDmZBtVmmnBWcK63390924 = fnDmZBtVmmnBWcK43134198;     fnDmZBtVmmnBWcK43134198 = fnDmZBtVmmnBWcK979065;     fnDmZBtVmmnBWcK979065 = fnDmZBtVmmnBWcK97091946;     fnDmZBtVmmnBWcK97091946 = fnDmZBtVmmnBWcK94238338;     fnDmZBtVmmnBWcK94238338 = fnDmZBtVmmnBWcK78789693;     fnDmZBtVmmnBWcK78789693 = fnDmZBtVmmnBWcK73178100;     fnDmZBtVmmnBWcK73178100 = fnDmZBtVmmnBWcK46233802;     fnDmZBtVmmnBWcK46233802 = fnDmZBtVmmnBWcK28582231;     fnDmZBtVmmnBWcK28582231 = fnDmZBtVmmnBWcK6141965;     fnDmZBtVmmnBWcK6141965 = fnDmZBtVmmnBWcK65129414;     fnDmZBtVmmnBWcK65129414 = fnDmZBtVmmnBWcK36482637;     fnDmZBtVmmnBWcK36482637 = fnDmZBtVmmnBWcK97040791;     fnDmZBtVmmnBWcK97040791 = fnDmZBtVmmnBWcK63939405;     fnDmZBtVmmnBWcK63939405 = fnDmZBtVmmnBWcK92092573;     fnDmZBtVmmnBWcK92092573 = fnDmZBtVmmnBWcK49768589;     fnDmZBtVmmnBWcK49768589 = fnDmZBtVmmnBWcK84667666;     fnDmZBtVmmnBWcK84667666 = fnDmZBtVmmnBWcK85009396;     fnDmZBtVmmnBWcK85009396 = fnDmZBtVmmnBWcK89675676;     fnDmZBtVmmnBWcK89675676 = fnDmZBtVmmnBWcK41792673;     fnDmZBtVmmnBWcK41792673 = fnDmZBtVmmnBWcK39190070;     fnDmZBtVmmnBWcK39190070 = fnDmZBtVmmnBWcK82248352;     fnDmZBtVmmnBWcK82248352 = fnDmZBtVmmnBWcK8395541;     fnDmZBtVmmnBWcK8395541 = fnDmZBtVmmnBWcK23990608;     fnDmZBtVmmnBWcK23990608 = fnDmZBtVmmnBWcK69408570;     fnDmZBtVmmnBWcK69408570 = fnDmZBtVmmnBWcK72324592;     fnDmZBtVmmnBWcK72324592 = fnDmZBtVmmnBWcK3125452;     fnDmZBtVmmnBWcK3125452 = fnDmZBtVmmnBWcK52811488;     fnDmZBtVmmnBWcK52811488 = fnDmZBtVmmnBWcK14885517;     fnDmZBtVmmnBWcK14885517 = fnDmZBtVmmnBWcK53087726;     fnDmZBtVmmnBWcK53087726 = fnDmZBtVmmnBWcK67213381;     fnDmZBtVmmnBWcK67213381 = fnDmZBtVmmnBWcK9906616;     fnDmZBtVmmnBWcK9906616 = fnDmZBtVmmnBWcK77669085;     fnDmZBtVmmnBWcK77669085 = fnDmZBtVmmnBWcK90963111;     fnDmZBtVmmnBWcK90963111 = fnDmZBtVmmnBWcK18295900;     fnDmZBtVmmnBWcK18295900 = fnDmZBtVmmnBWcK54712287;     fnDmZBtVmmnBWcK54712287 = fnDmZBtVmmnBWcK91488824;     fnDmZBtVmmnBWcK91488824 = fnDmZBtVmmnBWcK24827871;     fnDmZBtVmmnBWcK24827871 = fnDmZBtVmmnBWcK56440555;     fnDmZBtVmmnBWcK56440555 = fnDmZBtVmmnBWcK71283923;     fnDmZBtVmmnBWcK71283923 = fnDmZBtVmmnBWcK97679774;     fnDmZBtVmmnBWcK97679774 = fnDmZBtVmmnBWcK21671612;     fnDmZBtVmmnBWcK21671612 = fnDmZBtVmmnBWcK31818359;     fnDmZBtVmmnBWcK31818359 = fnDmZBtVmmnBWcK33941235;     fnDmZBtVmmnBWcK33941235 = fnDmZBtVmmnBWcK35692312;     fnDmZBtVmmnBWcK35692312 = fnDmZBtVmmnBWcK23574785;     fnDmZBtVmmnBWcK23574785 = fnDmZBtVmmnBWcK70066039;     fnDmZBtVmmnBWcK70066039 = fnDmZBtVmmnBWcK45773804;     fnDmZBtVmmnBWcK45773804 = fnDmZBtVmmnBWcK33754805;     fnDmZBtVmmnBWcK33754805 = fnDmZBtVmmnBWcK9988055;     fnDmZBtVmmnBWcK9988055 = fnDmZBtVmmnBWcK99858809;     fnDmZBtVmmnBWcK99858809 = fnDmZBtVmmnBWcK36527883;     fnDmZBtVmmnBWcK36527883 = fnDmZBtVmmnBWcK14615856;     fnDmZBtVmmnBWcK14615856 = fnDmZBtVmmnBWcK69429803;     fnDmZBtVmmnBWcK69429803 = fnDmZBtVmmnBWcK52125965;     fnDmZBtVmmnBWcK52125965 = fnDmZBtVmmnBWcK81142571;     fnDmZBtVmmnBWcK81142571 = fnDmZBtVmmnBWcK34738657;     fnDmZBtVmmnBWcK34738657 = fnDmZBtVmmnBWcK76988456;     fnDmZBtVmmnBWcK76988456 = fnDmZBtVmmnBWcK27683376;     fnDmZBtVmmnBWcK27683376 = fnDmZBtVmmnBWcK21913746;     fnDmZBtVmmnBWcK21913746 = fnDmZBtVmmnBWcK75664241;     fnDmZBtVmmnBWcK75664241 = fnDmZBtVmmnBWcK20366612;     fnDmZBtVmmnBWcK20366612 = fnDmZBtVmmnBWcK31348285;     fnDmZBtVmmnBWcK31348285 = fnDmZBtVmmnBWcK75494504;     fnDmZBtVmmnBWcK75494504 = fnDmZBtVmmnBWcK38928583;     fnDmZBtVmmnBWcK38928583 = fnDmZBtVmmnBWcK55222798;     fnDmZBtVmmnBWcK55222798 = fnDmZBtVmmnBWcK58813551;     fnDmZBtVmmnBWcK58813551 = fnDmZBtVmmnBWcK6077680;     fnDmZBtVmmnBWcK6077680 = fnDmZBtVmmnBWcK45643505;     fnDmZBtVmmnBWcK45643505 = fnDmZBtVmmnBWcK37380287;     fnDmZBtVmmnBWcK37380287 = fnDmZBtVmmnBWcK58279765;     fnDmZBtVmmnBWcK58279765 = fnDmZBtVmmnBWcK59839795;     fnDmZBtVmmnBWcK59839795 = fnDmZBtVmmnBWcK28568842;     fnDmZBtVmmnBWcK28568842 = fnDmZBtVmmnBWcK18391753;     fnDmZBtVmmnBWcK18391753 = fnDmZBtVmmnBWcK44112899;     fnDmZBtVmmnBWcK44112899 = fnDmZBtVmmnBWcK17518459;     fnDmZBtVmmnBWcK17518459 = fnDmZBtVmmnBWcK50429994;     fnDmZBtVmmnBWcK50429994 = fnDmZBtVmmnBWcK74454306;     fnDmZBtVmmnBWcK74454306 = fnDmZBtVmmnBWcK88298296;     fnDmZBtVmmnBWcK88298296 = fnDmZBtVmmnBWcK45833786;     fnDmZBtVmmnBWcK45833786 = fnDmZBtVmmnBWcK2258554;     fnDmZBtVmmnBWcK2258554 = fnDmZBtVmmnBWcK57351647;     fnDmZBtVmmnBWcK57351647 = fnDmZBtVmmnBWcK19056683;     fnDmZBtVmmnBWcK19056683 = fnDmZBtVmmnBWcK4897463;     fnDmZBtVmmnBWcK4897463 = fnDmZBtVmmnBWcK53228916;     fnDmZBtVmmnBWcK53228916 = fnDmZBtVmmnBWcK30685499;     fnDmZBtVmmnBWcK30685499 = fnDmZBtVmmnBWcK95290760;     fnDmZBtVmmnBWcK95290760 = fnDmZBtVmmnBWcK8239283;     fnDmZBtVmmnBWcK8239283 = fnDmZBtVmmnBWcK38837147;     fnDmZBtVmmnBWcK38837147 = fnDmZBtVmmnBWcK37153328;     fnDmZBtVmmnBWcK37153328 = fnDmZBtVmmnBWcK19973631;     fnDmZBtVmmnBWcK19973631 = fnDmZBtVmmnBWcK14500368;     fnDmZBtVmmnBWcK14500368 = fnDmZBtVmmnBWcK97144494;     fnDmZBtVmmnBWcK97144494 = fnDmZBtVmmnBWcK34526809;     fnDmZBtVmmnBWcK34526809 = fnDmZBtVmmnBWcK95619682;     fnDmZBtVmmnBWcK95619682 = fnDmZBtVmmnBWcK77313162;     fnDmZBtVmmnBWcK77313162 = fnDmZBtVmmnBWcK90323326;     fnDmZBtVmmnBWcK90323326 = fnDmZBtVmmnBWcK4291532;}
// Junk Finished

// Junk Code By Peatreat & Thaisen's Gen
void tVrsbRUYCRcEZmqr9764391() {     float YVfolJACHCwpxQo65342248 = -399223467;    float YVfolJACHCwpxQo82319381 = -48295088;    float YVfolJACHCwpxQo60268475 = -725835347;    float YVfolJACHCwpxQo96256348 = -295559563;    float YVfolJACHCwpxQo59921370 = 19706134;    float YVfolJACHCwpxQo19405781 = -616462791;    float YVfolJACHCwpxQo44522862 = -662340770;    float YVfolJACHCwpxQo2165973 = -751947534;    float YVfolJACHCwpxQo22896427 = -730700224;    float YVfolJACHCwpxQo10599682 = -174892202;    float YVfolJACHCwpxQo78428485 = -94498420;    float YVfolJACHCwpxQo50176358 = -193865378;    float YVfolJACHCwpxQo89104473 = -913919821;    float YVfolJACHCwpxQo55985774 = -267896301;    float YVfolJACHCwpxQo5773932 = -766130275;    float YVfolJACHCwpxQo13738589 = -863615677;    float YVfolJACHCwpxQo63720697 = -495334284;    float YVfolJACHCwpxQo89968137 = -976426693;    float YVfolJACHCwpxQo91355370 = -180116474;    float YVfolJACHCwpxQo67592588 = -774566771;    float YVfolJACHCwpxQo19436101 = -17549053;    float YVfolJACHCwpxQo16304042 = -751455873;    float YVfolJACHCwpxQo79203454 = -132092526;    float YVfolJACHCwpxQo91046949 = -829500652;    float YVfolJACHCwpxQo16726754 = -708470544;    float YVfolJACHCwpxQo32008773 = -705175551;    float YVfolJACHCwpxQo96577509 = 22094379;    float YVfolJACHCwpxQo38632271 = -598994207;    float YVfolJACHCwpxQo14191573 = -682744955;    float YVfolJACHCwpxQo85891483 = -578337050;    float YVfolJACHCwpxQo60028675 = 90928077;    float YVfolJACHCwpxQo5174611 = -404452705;    float YVfolJACHCwpxQo1040366 = -658675800;    float YVfolJACHCwpxQo50266042 = -945010600;    float YVfolJACHCwpxQo94762019 = -358054693;    float YVfolJACHCwpxQo91193590 = -778536071;    float YVfolJACHCwpxQo78078946 = -59437996;    float YVfolJACHCwpxQo75166310 = 55690344;    float YVfolJACHCwpxQo90712831 = 24438963;    float YVfolJACHCwpxQo92671137 = -696366357;    float YVfolJACHCwpxQo17575092 = 93188602;    float YVfolJACHCwpxQo4117611 = -324710788;    float YVfolJACHCwpxQo49584161 = -977347991;    float YVfolJACHCwpxQo49372494 = -65792528;    float YVfolJACHCwpxQo29650894 = -302539635;    float YVfolJACHCwpxQo3752041 = -837280322;    float YVfolJACHCwpxQo25357060 = -988648564;    float YVfolJACHCwpxQo86147813 = -680099825;    float YVfolJACHCwpxQo78620381 = -543067154;    float YVfolJACHCwpxQo28360200 = -368112799;    float YVfolJACHCwpxQo23421406 = -276136207;    float YVfolJACHCwpxQo29951600 = -845997993;    float YVfolJACHCwpxQo21234458 = -581512133;    float YVfolJACHCwpxQo74499294 = -138616861;    float YVfolJACHCwpxQo90328290 = -262083741;    float YVfolJACHCwpxQo49038206 = -647767595;    float YVfolJACHCwpxQo3115927 = -916202563;    float YVfolJACHCwpxQo69221526 = -896334695;    float YVfolJACHCwpxQo79529594 = -587089020;    float YVfolJACHCwpxQo27912597 = -275118316;    float YVfolJACHCwpxQo22828271 = -538557170;    float YVfolJACHCwpxQo5890591 = 36653437;    float YVfolJACHCwpxQo87974400 = 30797421;    float YVfolJACHCwpxQo37004944 = -52363174;    float YVfolJACHCwpxQo50571006 = -165820279;    float YVfolJACHCwpxQo73253875 = -690045716;    float YVfolJACHCwpxQo49135992 = -535189578;    float YVfolJACHCwpxQo38838432 = -968909221;    float YVfolJACHCwpxQo61223754 = -909841608;    float YVfolJACHCwpxQo14580342 = -987594204;    float YVfolJACHCwpxQo35659643 = -704177681;    float YVfolJACHCwpxQo88554386 = -451024628;    float YVfolJACHCwpxQo99255306 = -900865657;    float YVfolJACHCwpxQo98684233 = -483750118;    float YVfolJACHCwpxQo50017496 = -767755373;    float YVfolJACHCwpxQo15318490 = -692838266;    float YVfolJACHCwpxQo66719881 = -774107882;    float YVfolJACHCwpxQo29830961 = 33700001;    float YVfolJACHCwpxQo61396055 = -426961018;    float YVfolJACHCwpxQo12974713 = -871190222;    float YVfolJACHCwpxQo6651713 = -716526988;    float YVfolJACHCwpxQo10429697 = -297805797;    float YVfolJACHCwpxQo60011889 = 44072946;    float YVfolJACHCwpxQo85831372 = -214632157;    float YVfolJACHCwpxQo62470077 = -202200843;    float YVfolJACHCwpxQo30077076 = -63073930;    float YVfolJACHCwpxQo83940152 = -822940572;    float YVfolJACHCwpxQo26541072 = -420058940;    float YVfolJACHCwpxQo59937752 = -582926860;    float YVfolJACHCwpxQo45723813 = -710287099;    float YVfolJACHCwpxQo88077663 = -862333509;    float YVfolJACHCwpxQo8857420 = -163103302;    float YVfolJACHCwpxQo95636715 = -357220637;    float YVfolJACHCwpxQo62800234 = -700442722;    float YVfolJACHCwpxQo69842866 = -57809188;    float YVfolJACHCwpxQo11684501 = -943464836;    float YVfolJACHCwpxQo16143211 = -255508209;    float YVfolJACHCwpxQo12579218 = -824984817;    float YVfolJACHCwpxQo98801488 = -899972249;    float YVfolJACHCwpxQo56397019 = -399223467;     YVfolJACHCwpxQo65342248 = YVfolJACHCwpxQo82319381;     YVfolJACHCwpxQo82319381 = YVfolJACHCwpxQo60268475;     YVfolJACHCwpxQo60268475 = YVfolJACHCwpxQo96256348;     YVfolJACHCwpxQo96256348 = YVfolJACHCwpxQo59921370;     YVfolJACHCwpxQo59921370 = YVfolJACHCwpxQo19405781;     YVfolJACHCwpxQo19405781 = YVfolJACHCwpxQo44522862;     YVfolJACHCwpxQo44522862 = YVfolJACHCwpxQo2165973;     YVfolJACHCwpxQo2165973 = YVfolJACHCwpxQo22896427;     YVfolJACHCwpxQo22896427 = YVfolJACHCwpxQo10599682;     YVfolJACHCwpxQo10599682 = YVfolJACHCwpxQo78428485;     YVfolJACHCwpxQo78428485 = YVfolJACHCwpxQo50176358;     YVfolJACHCwpxQo50176358 = YVfolJACHCwpxQo89104473;     YVfolJACHCwpxQo89104473 = YVfolJACHCwpxQo55985774;     YVfolJACHCwpxQo55985774 = YVfolJACHCwpxQo5773932;     YVfolJACHCwpxQo5773932 = YVfolJACHCwpxQo13738589;     YVfolJACHCwpxQo13738589 = YVfolJACHCwpxQo63720697;     YVfolJACHCwpxQo63720697 = YVfolJACHCwpxQo89968137;     YVfolJACHCwpxQo89968137 = YVfolJACHCwpxQo91355370;     YVfolJACHCwpxQo91355370 = YVfolJACHCwpxQo67592588;     YVfolJACHCwpxQo67592588 = YVfolJACHCwpxQo19436101;     YVfolJACHCwpxQo19436101 = YVfolJACHCwpxQo16304042;     YVfolJACHCwpxQo16304042 = YVfolJACHCwpxQo79203454;     YVfolJACHCwpxQo79203454 = YVfolJACHCwpxQo91046949;     YVfolJACHCwpxQo91046949 = YVfolJACHCwpxQo16726754;     YVfolJACHCwpxQo16726754 = YVfolJACHCwpxQo32008773;     YVfolJACHCwpxQo32008773 = YVfolJACHCwpxQo96577509;     YVfolJACHCwpxQo96577509 = YVfolJACHCwpxQo38632271;     YVfolJACHCwpxQo38632271 = YVfolJACHCwpxQo14191573;     YVfolJACHCwpxQo14191573 = YVfolJACHCwpxQo85891483;     YVfolJACHCwpxQo85891483 = YVfolJACHCwpxQo60028675;     YVfolJACHCwpxQo60028675 = YVfolJACHCwpxQo5174611;     YVfolJACHCwpxQo5174611 = YVfolJACHCwpxQo1040366;     YVfolJACHCwpxQo1040366 = YVfolJACHCwpxQo50266042;     YVfolJACHCwpxQo50266042 = YVfolJACHCwpxQo94762019;     YVfolJACHCwpxQo94762019 = YVfolJACHCwpxQo91193590;     YVfolJACHCwpxQo91193590 = YVfolJACHCwpxQo78078946;     YVfolJACHCwpxQo78078946 = YVfolJACHCwpxQo75166310;     YVfolJACHCwpxQo75166310 = YVfolJACHCwpxQo90712831;     YVfolJACHCwpxQo90712831 = YVfolJACHCwpxQo92671137;     YVfolJACHCwpxQo92671137 = YVfolJACHCwpxQo17575092;     YVfolJACHCwpxQo17575092 = YVfolJACHCwpxQo4117611;     YVfolJACHCwpxQo4117611 = YVfolJACHCwpxQo49584161;     YVfolJACHCwpxQo49584161 = YVfolJACHCwpxQo49372494;     YVfolJACHCwpxQo49372494 = YVfolJACHCwpxQo29650894;     YVfolJACHCwpxQo29650894 = YVfolJACHCwpxQo3752041;     YVfolJACHCwpxQo3752041 = YVfolJACHCwpxQo25357060;     YVfolJACHCwpxQo25357060 = YVfolJACHCwpxQo86147813;     YVfolJACHCwpxQo86147813 = YVfolJACHCwpxQo78620381;     YVfolJACHCwpxQo78620381 = YVfolJACHCwpxQo28360200;     YVfolJACHCwpxQo28360200 = YVfolJACHCwpxQo23421406;     YVfolJACHCwpxQo23421406 = YVfolJACHCwpxQo29951600;     YVfolJACHCwpxQo29951600 = YVfolJACHCwpxQo21234458;     YVfolJACHCwpxQo21234458 = YVfolJACHCwpxQo74499294;     YVfolJACHCwpxQo74499294 = YVfolJACHCwpxQo90328290;     YVfolJACHCwpxQo90328290 = YVfolJACHCwpxQo49038206;     YVfolJACHCwpxQo49038206 = YVfolJACHCwpxQo3115927;     YVfolJACHCwpxQo3115927 = YVfolJACHCwpxQo69221526;     YVfolJACHCwpxQo69221526 = YVfolJACHCwpxQo79529594;     YVfolJACHCwpxQo79529594 = YVfolJACHCwpxQo27912597;     YVfolJACHCwpxQo27912597 = YVfolJACHCwpxQo22828271;     YVfolJACHCwpxQo22828271 = YVfolJACHCwpxQo5890591;     YVfolJACHCwpxQo5890591 = YVfolJACHCwpxQo87974400;     YVfolJACHCwpxQo87974400 = YVfolJACHCwpxQo37004944;     YVfolJACHCwpxQo37004944 = YVfolJACHCwpxQo50571006;     YVfolJACHCwpxQo50571006 = YVfolJACHCwpxQo73253875;     YVfolJACHCwpxQo73253875 = YVfolJACHCwpxQo49135992;     YVfolJACHCwpxQo49135992 = YVfolJACHCwpxQo38838432;     YVfolJACHCwpxQo38838432 = YVfolJACHCwpxQo61223754;     YVfolJACHCwpxQo61223754 = YVfolJACHCwpxQo14580342;     YVfolJACHCwpxQo14580342 = YVfolJACHCwpxQo35659643;     YVfolJACHCwpxQo35659643 = YVfolJACHCwpxQo88554386;     YVfolJACHCwpxQo88554386 = YVfolJACHCwpxQo99255306;     YVfolJACHCwpxQo99255306 = YVfolJACHCwpxQo98684233;     YVfolJACHCwpxQo98684233 = YVfolJACHCwpxQo50017496;     YVfolJACHCwpxQo50017496 = YVfolJACHCwpxQo15318490;     YVfolJACHCwpxQo15318490 = YVfolJACHCwpxQo66719881;     YVfolJACHCwpxQo66719881 = YVfolJACHCwpxQo29830961;     YVfolJACHCwpxQo29830961 = YVfolJACHCwpxQo61396055;     YVfolJACHCwpxQo61396055 = YVfolJACHCwpxQo12974713;     YVfolJACHCwpxQo12974713 = YVfolJACHCwpxQo6651713;     YVfolJACHCwpxQo6651713 = YVfolJACHCwpxQo10429697;     YVfolJACHCwpxQo10429697 = YVfolJACHCwpxQo60011889;     YVfolJACHCwpxQo60011889 = YVfolJACHCwpxQo85831372;     YVfolJACHCwpxQo85831372 = YVfolJACHCwpxQo62470077;     YVfolJACHCwpxQo62470077 = YVfolJACHCwpxQo30077076;     YVfolJACHCwpxQo30077076 = YVfolJACHCwpxQo83940152;     YVfolJACHCwpxQo83940152 = YVfolJACHCwpxQo26541072;     YVfolJACHCwpxQo26541072 = YVfolJACHCwpxQo59937752;     YVfolJACHCwpxQo59937752 = YVfolJACHCwpxQo45723813;     YVfolJACHCwpxQo45723813 = YVfolJACHCwpxQo88077663;     YVfolJACHCwpxQo88077663 = YVfolJACHCwpxQo8857420;     YVfolJACHCwpxQo8857420 = YVfolJACHCwpxQo95636715;     YVfolJACHCwpxQo95636715 = YVfolJACHCwpxQo62800234;     YVfolJACHCwpxQo62800234 = YVfolJACHCwpxQo69842866;     YVfolJACHCwpxQo69842866 = YVfolJACHCwpxQo11684501;     YVfolJACHCwpxQo11684501 = YVfolJACHCwpxQo16143211;     YVfolJACHCwpxQo16143211 = YVfolJACHCwpxQo12579218;     YVfolJACHCwpxQo12579218 = YVfolJACHCwpxQo98801488;     YVfolJACHCwpxQo98801488 = YVfolJACHCwpxQo56397019;     YVfolJACHCwpxQo56397019 = YVfolJACHCwpxQo65342248;}
// Junk Finished
