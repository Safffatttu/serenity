/*
 * Copyright (c) 2020, Andreas Kling <kling@serenityos.org>
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice, this
 *    list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include <AK/String.h>
#include <AK/StringBuilder.h>
#include <AK/Vector.h>
#include <LibTextCodec/Decoder.h>

namespace TextCodec {

static Latin1Decoder& latin1_decoder()
{
    static Latin1Decoder* decoder;
    if (!decoder)
        decoder = new Latin1Decoder;
    return *decoder;
}

static UTF8Decoder& utf8_decoder()
{
    static UTF8Decoder* decoder;
    if (!decoder)
        decoder = new UTF8Decoder;
    return *decoder;
}

static ISO_8859_2Decoder& ISO_8859_2_decoder()
{
    static ISO_8859_2Decoder* decoder;
    if (!decoder)
        decoder = new ISO_8859_2Decoder;
    return *decoder;
}

Decoder* decoder_for(const String& encoding)
{
    ASSERT(is_standardized_encoding(encoding));
    if (encoding.equals_ignoring_case("windows-1252"))
        return &latin1_decoder();
    if (encoding.equals_ignoring_case("utf-8"))
        return &utf8_decoder();
    if (encoding.equals_ignoring_case("ISO-8859-2")) {
        dbg() << "USE NEW ENCODING";
        return &ISO_8859_2_decoder();
    }

    dbg() << "TextCodec: No decoder implemented for encoding '" << encoding << "'";
    return nullptr;
}

// https://encoding.spec.whatwg.org/#concept-encoding-get
String get_standardized_encoding(const String& encoding)
{
    String trimmed_lowercase_encoding = encoding.trim_whitespace().to_lowercase();

    if (trimmed_lowercase_encoding.is_one_of("unicode-1-1-utf-8", "unicode11utf8", "unicode20utf8", "utf-8", "utf8", "x-unicode20utf8"))
        return "UTF-8";
    if (trimmed_lowercase_encoding.is_one_of("866", "cp866", "csibm866", "ibm666"))
        return "IBM666";
    if (trimmed_lowercase_encoding.is_one_of("csisolatin2", "iso-8859-2", "iso-ir-101", "iso8859-2", "iso88592", "iso_8859-2", "iso_8859-2:1987", "l2", "latin2"))
        return "ISO-8859-2";
    if (trimmed_lowercase_encoding.is_one_of("csisolatin3", "iso-8859-3", "iso-ir-109", "iso8859-3", "iso88593", "iso_8859-3", "iso_8859-3:1988", "l3", "latin3"))
        return "ISO-8859-3";
    if (trimmed_lowercase_encoding.is_one_of("csisolatin4", "iso-8859-4", "iso-ir-110", "iso8859-4", "iso88594", "iso_8859-4", "iso_8859-4:1989", "l4", "latin4"))
        return "ISO-8859-4";
    if (trimmed_lowercase_encoding.is_one_of("csisolatincyrillic", "cyrillic", "iso-8859-5", "iso-ir-144", "iso8859-5", "iso88595", "iso_8859-5", "iso_8859-5:1988"))
        return "ISO-8859-5";
    if (trimmed_lowercase_encoding.is_one_of("arabic", "asmo-708", "csiso88596e", "csiso88596i", "csisolatinarabic", "ecma-114", "iso-8859-6", "iso-8859-6-e", "iso-8859-6-i", "iso-ir-127", "iso8859-6", "iso88596", "iso_8859-6", "iso_8859-6:1987"))
        return "ISO-8859-6";
    if (trimmed_lowercase_encoding.is_one_of("csisolatingreek", "ecma-118", "elot_928", "greek", "greek8", "iso-8859-7", "iso-ir-126", "iso8859-7", "iso88597", "iso_8859-7", "iso_8859-7:1987", "sun_eu_greek"))
        return "ISO-8859-7";
    if (trimmed_lowercase_encoding.is_one_of("csiso88598e", "csisolatinhebrew", "hebrew", "iso-8859-8", "iso-8859-8-e", "iso-ir-138", "iso8859-8", "iso88598", "iso_8859-8", "iso_8859-8:1988", "visual"))
        return "ISO-8859-8";
    if (trimmed_lowercase_encoding.is_one_of("csiso88598i", "iso-8859-8-i", "logical"))
        return "ISO-8859-8-I";
    if (trimmed_lowercase_encoding.is_one_of("csisolatin6", "iso8859-10", "iso-ir-157", "iso8859-10", "iso885910", "l6", "latin6"))
        return "ISO-8859-10";
    if (trimmed_lowercase_encoding.is_one_of("iso-8859-13", "iso8859-13", "iso885913"))
        return "ISO-8859-13";
    if (trimmed_lowercase_encoding.is_one_of("iso-8859-14", "iso8859-14", "iso885914"))
        return "ISO-8859-14";
    if (trimmed_lowercase_encoding.is_one_of("csisolatin9", "iso-8859-15", "iso8859-15", "iso885915", "iso_8859-15", "l9"))
        return "ISO-8859-15";
    if (trimmed_lowercase_encoding == "iso-8859-16")
        return "ISO-8859-16";
    if (trimmed_lowercase_encoding.is_one_of("cskoi8r", "koi", "koi8", "koi8-r", "koi8_r"))
        return "KOI8-R";
    if (trimmed_lowercase_encoding.is_one_of("koi8-ru", "koi8-u"))
        return "KOI8-U";
    if (trimmed_lowercase_encoding.is_one_of("csmacintosh", "mac", "macintosh", "x-mac-roman"))
        return "macintosh";
    if (trimmed_lowercase_encoding.is_one_of("dos-874", "iso-8859-11", "iso8859-11", "iso885911", "tis-620", "windows-874"))
        return "windows-874";
    if (trimmed_lowercase_encoding.is_one_of("cp1250", "windows-1250", "x-cp1250"))
        return "windows-1250";
    if (trimmed_lowercase_encoding.is_one_of("cp1251", "windows-1251", "x-cp1251"))
        return "windows-1251";
    if (trimmed_lowercase_encoding.is_one_of("ansi_x3.4-1968", "ascii", "cp1252", "cp819", "csisolatin1", "ibm819", "iso-8859-1", "iso-ir-100", "iso8859-1", "iso88591", "iso_8859-1", "iso_8859-1:1987", "l1", "latin1", "us-ascii", "windows-1252", "x-cp1252"))
        return "windows-1252";
    if (trimmed_lowercase_encoding.is_one_of("cp1253", "windows-1253", "x-cp1253"))
        return "windows-1253";
    if (trimmed_lowercase_encoding.is_one_of("cp1254", "csisolatin5", "iso-8859-9", "iso-ir-148", "iso-8859-9", "iso-88599", "iso_8859-9", "iso_8859-9:1989", "l5", "latin5", "windows-1254", "x-cp1254"))
        return "windows-1254";
    if (trimmed_lowercase_encoding.is_one_of("cp1255", "windows-1255", "x-cp1255"))
        return "windows-1255";
    if (trimmed_lowercase_encoding.is_one_of("cp1256", "windows-1256", "x-cp1256"))
        return "windows-1256";
    if (trimmed_lowercase_encoding.is_one_of("cp1257", "windows-1257", "x-cp1257"))
        return "windows-1257";
    if (trimmed_lowercase_encoding.is_one_of("cp1258", "windows-1258", "x-cp1258"))
        return "windows-1258";
    if (trimmed_lowercase_encoding.is_one_of("x-mac-cyrillic", "x-mac-ukrainian"))
        return "x-mac-cyrillic";
    if (trimmed_lowercase_encoding.is_one_of("chinese", "csgb2312", "csiso58gb231280", "gb2312", "gb_2312", "gb_2312-80", "gbk", "iso-ir-58", "x-gbk"))
        return "GBK";
    if (trimmed_lowercase_encoding == "gb18030")
        return "gb18030";
    if (trimmed_lowercase_encoding.is_one_of("big5", "big5-hkscs", "cn-big5", "csbig5", "x-x-big5"))
        return "Big5";
    if (trimmed_lowercase_encoding.is_one_of("cseucpkdfmtjapanese", "euc-jp", "x-euc-jp"))
        return "EUC-JP";
    if (trimmed_lowercase_encoding.is_one_of("csiso2022jp", "iso-2022-jp"))
        return "ISO-2022-JP";
    if (trimmed_lowercase_encoding.is_one_of("csshiftjis", "ms932", "ms_kanji", "shift-jis", "shift_jis", "sjis", "windows-31j", "x-sjis"))
        return "Shift_JIS";
    if (trimmed_lowercase_encoding.is_one_of("cseuckr", "csksc56011987", "euc-kr", "iso-ir-149", "korean", "ks_c_5601-1987", "ks_c_5601-1989", "ksc5601", "ksc_5601", "windows-949"))
        return "EUC-KR";
    if (trimmed_lowercase_encoding.is_one_of("csiso2022kr", "hz-gb-2312", "iso-2022-cn", "iso-2022-cn-ext", "iso-2022-kr", "replacement"))
        return "replacement";
    if (trimmed_lowercase_encoding.is_one_of("unicodefffe", "utf-16be"))
        return "UTF-16BE";
    if (trimmed_lowercase_encoding.is_one_of("csunicode", "iso-10646-ucs-2", "ucs-2", "unicode", "unicodefeff", "utf-16", "utf-16le"))
        return "UTF-16LE";
    if (trimmed_lowercase_encoding == "x-user-defined")
        return "x-user-defined";

    dbg() << "TextCodec: Unrecognized encoding: " << encoding;
    return {};
}

bool is_standardized_encoding(const String& encoding)
{
    auto lowercase_encoding = encoding.to_lowercase();
    return lowercase_encoding.is_one_of("utf-8", "ibm666", "iso-8859-2", "iso-8859-3", "iso-8859-4", "iso-8859-5", "iso-8859-6", "iso-8859-7", "iso-8859-8", "iso-8859-8-I", "iso-8859-10", "iso-8859-13", "iso-8859-14", "iso-8859-15", "iso-8859-16", "koi8-r", "koi8-u", "macintosh", "windows-874", "windows-1250", "windows-1251", "windows-1252", "windows-1253", "windows-1254", "windows-1255", "windows-1256", "windows-1257", "windows-1258", "x-mac-cyrillic", "gbk", "gb18030", "big5", "euc-jp", "iso-2022-JP", "shift_jis", "euc-kr", "replacement", "utf-16be", "utf-16le", "x-user-defined");
}

String UTF8Decoder::to_utf8(const StringView& input)
{
    return input;
}

String Latin1Decoder::to_utf8(const StringView& input)
{
    StringBuilder builder(input.length());
    for (size_t i = 0; i < input.length(); ++i) {
        u8 ch = input[i];
        // Latin1 is the same as the first 256 Unicode code_points, so no mapping is needed, just utf-8 encoding.
        builder.append_code_point(ch);
    }
    return builder.to_string();
}

String ISO_8859_2Decoder::to_utf8(const StringView& input)
{
    const static Vector<u32> mapping = {
        0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
        0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f,
        0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17,
        0x18, 0x19, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f,
        0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27,
        0x28, 0x29, 0x2a, 0x2b, 0x2c, 0x2d, 0x2e, 0x2f,
        0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37,
        0x38, 0x39, 0x3a, 0x3b, 0x3c, 0x3d, 0x3e, 0x3f,
        0x40, 0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47,
        0x48, 0x49, 0x4a, 0x4b, 0x4c, 0x4d, 0x4e, 0x4f,
        0x50, 0x51, 0x52, 0x53, 0x54, 0x55, 0x56, 0x57,
        0x58, 0x59, 0x5a, 0x5b, 0x5c, 0x5d, 0x5e, 0x5f,
        0x60, 0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67,
        0x68, 0x69, 0x6a, 0x6b, 0x6c, 0x6d, 0x6e, 0x6f,
        0x70, 0x71, 0x72, 0x73, 0x74, 0x75, 0x76, 0x77,
        0x78, 0x79, 0x7a, 0x7b, 0x7c, 0x7d, 0x7e, 0x7f,
        0x80, 0x81, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87,
        0x88, 0x89, 0x8a, 0x8b, 0x8c, 0x8d, 0x8e, 0x8f,
        0x90, 0x91, 0x92, 0x93, 0x94, 0x95, 0x96, 0x97,
        0x98, 0x99, 0x9a, 0x9b, 0x9c, 0x9d, 0x9e, 0x9f,
        0xa0, 0x104, 0x2d8, 0x141, 0xa4, 0x13d, 0x15a, 0xa7,
        0xa8, 0x160, 0x15e, 0x164, 0x179, 0xad, 0x17d, 0x17b,
        0xb0, 0x105, 0x2db, 0x142, 0xb4, 0x13e, 0x15b, 0x2c7,
        0xb8, 0x161, 0x15f, 0x165, 0x17a, 0x2dd, 0x17e, 0x17c,
        0x154, 0xc1, 0xc2, 0x102, 0xc4, 0x139, 0x106, 0xc7,
        0x10c, 0xc9, 0x118, 0xcb, 0x11a, 0xcd, 0xce, 0x10e,
        0x110, 0x143, 0x147, 0xd3, 0xd4, 0x150, 0xd6, 0xd7,
        0x158, 0x16e, 0xda, 0x170, 0xdc, 0xdd, 0x162, 0xdf,
        0x155, 0xe1, 0xe2, 0x103, 0xe4, 0x13a, 0x107, 0xe7,
        0x10d, 0xe9, 0x119, 0xeb, 0x11b, 0xed, 0xee, 0x10f,
        0x111, 0x144, 0x148, 0xf3, 0xf4, 0x151, 0xf6, 0xf7,
        0x159, 0x16f, 0xfa, 0x171, 0xfc, 0xfd, 0x163, 0x2d9
    };

    StringBuilder builder(input.length());

    for (size_t i = 0; i < input.length(); ++i) {
        const u8 ch = input[i];
        dbg() << ch << input[i];

        builder.append_code_point(mapping[ch]);
    }

    return builder.to_string();
}

}
