/*
*******************************************************************************
* Copyright (C) 2009-2010, Yahoo! Inc.
* All Rights Reserved.
* Copyright (C) 2009, Yahoo! Inc.
*******************************************************************************
*

* File SELFMT.H
*
* Modification History:
*
*   Date        Name        Description
*   11/11/09    kirtig      Finished first cut of implementation.
********************************************************************************
*/

#ifndef SELFMT
#define SELFMT

#include "unicode/utypes.h"
#include "unicode/numfmt.h"

/**
 * \file
 * \brief C++ API: SelectFormat object
 */

#if !UCONFIG_NO_FORMATTING

U_NAMESPACE_BEGIN

class Hashtable;

/**
  * <p><code>SelectFormat</code> supports the creation of  internationalized
  * messages by selecting phrases based on keywords. The pattern  specifies
  * how to map keywords to phrases and provides a default phrase. The
  * object provided to the format method is a string that's matched
  * against the keywords. If there is a match, the corresponding phrase
  * is selected; otherwise, the default phrase is used.</p>
  *
  * <h4>Using <code>SelectFormat</code> for Gender Agreement</h4>
  *
  * <p>The main use case for the select format is gender based  inflection.
  * When names or nouns are inserted into sentences, their gender can  affect pronouns,
  * verb forms, articles, and adjectives. Special care needs to be
  * taken for the case where the gender cannot be determined.
  * The impact varies between languages:</p>
  *
  * <ul>
  * <li>English has three genders, and unknown gender is handled as a  special
  * case. Names use the gender of the named person (if known), nouns  referring
  * to people use natural gender, and inanimate objects are usually  neutral.
  * The gender only affects pronouns: "he", "she", "it", "they".
  *
  * <li>German differs from English in that the gender of nouns is  rather
  * arbitrary, even for nouns referring to people ("Mädchen", girl, is  neutral).
  * The gender affects pronouns ("er", "sie", "es"), articles ("der",  "die",
  * "das"), and adjective forms ("guter Mann", "gute Frau", "gutes  Mädchen").
  *
  * <li>French has only two genders; as in German the gender of nouns
  * is rather arbitrary – for sun and moon, the genders
  * are the opposite of those in German. The gender affects
  * pronouns ("il", "elle"), articles ("le", "la"),
  * adjective forms ("bon", "bonne"), and sometimes
  * verb forms ("allé", "allée").
  *
  * <li>Polish distinguishes five genders (or noun classes),
  * human masculine, animate non-human masculine, inanimate masculine,
  * feminine, and neuter.
  * </ul>
  *
  * <p>Some other languages have noun classes that are not related to  gender,
  * but similar in grammatical use.
  * Some African languages have around 20 noun classes.</p>
  *
  * <p>To enable localizers to create sentence patterns that take their
  * language's gender dependencies into consideration, software has to  provide
  * information about the gender associated with a noun or name to
  * <code>MessageFormat</code>.
  * Two main cases can be distinguished:</p>
  *
  * <ul>
  * <li>For people, natural gender information should be maintained  for each person.
  * The keywords "male", "female", "mixed" (for groups of people)
  * and "unknown" are used.
  *
  * <li>For nouns, grammatical gender information should be maintained  for
  * each noun and per language, e.g., in resource bundles.
  * The keywords "masculine", "feminine", and "neuter" are commonly  used,
  * but some languages may require other keywords.
  * </ul>
  *
  * <p>The resulting keyword is provided to <code>MessageFormat</code>  as a
  * parameter separate from the name or noun it's associated with. For  example,
  * to generate a message such as "Jean went to Paris", three separate  arguments
  * would be provided: The name of the person as argument 0, the  gender of
  * the person as argument 1, and the name of the city as argument 2.
  * The sentence pattern for English, where the gender of the person has
  * no impact on this simple sentence, would not refer to argument 1  at all:</p>
  *
  * <pre>{0} went to {2}.</pre>
  *
  * <p>The sentence pattern for French, where the gender of the person affects
  * the form of the participle, uses a select format based on argument 1:</p>
  *
  * <pre>{0} est {1, select, female {allée} other {allé}} à {2}.</pre>
  *
  * <p>Patterns can be nested, so that it's possible to handle  interactions of
  * number and gender where necessary. For example, if the above  sentence should
  * allow for the names of several people to be inserted, the  following sentence
  * pattern can be used (with argument 0 the list of people's names,  
  * argument 1 the number of people, argument 2 their combined gender, and  
  * argument 3 the city name):</p>
  *
  * <pre>{0} {1, plural, 
  *                 one {est {2, select, female {allée} other  {allé}}}
  *                 other {sont {2, select, female {allées} other {allés}}}
  *          }à {3}.</pre>
  *
  * <h4>Patterns and Their Interpretation</h4>
  *
  * <p>The <code>SelectFormat</code> pattern text defines the phrase  output
  * for each user-defined keyword.
  * The pattern is a sequence of <code><i>keyword</i>{<i>phrase</i>}</code>
  * clauses, separated by white space characters.
  * Each clause assigns the phrase <code><i>phrase</i></code>
  * to the user-defined <code><i>keyword</i></code>.</p>
  *
  * <p>Keywords must match the pattern [a-zA-Z][a-zA-Z0-9_-]*; keywords
  * that don't match this pattern result in the error code
  * <code>U_ILLEGAL_CHARACTER</code>.
  * You always have to define a phrase for the default keyword
  * <code>other</code>; this phrase is returned when the keyword  
  * provided to
  * the <code>format</code> method matches no other keyword.
  * If a pattern does not provide a phrase for <code>other</code>, the  method
  * it's provided to returns the error  <code>U_DEFAULT_KEYWORD_MISSING</code>.
  * If a pattern provides more than one phrase for the same keyword, the
  * error <code>U_DUPLICATE_KEYWORD</code> is returned.
  * <br/>
  * Spaces between <code><i>keyword</i></code> and
  * <code>{<i>phrase</i>}</code>  will be ignored; spaces within
  * <code>{<i>phrase</i>}</code> will be preserved.<p>
  *
  * <p>The phrase for a particular select case may contain other message
  * format patterns. <code>SelectFormat</code> preserves these so that  you
  * can use the strings produced by <code>SelectFormat</code> with other
  * formatters. If you are using <code>SelectFormat</code> inside a
  * <code>MessageFormat</code> pattern, <code>MessageFormat</code> will
  * automatically evaluate the resulting format pattern.
  * Thus, curly braces (<code>{</code>, <code>}</code>) are <i>only</i> allowed
  * in phrases to define a nested format pattern.</p>
  *
  * <p>Example:
  * <pre>
  *
  * UErrorCode status = U_ZERO_ERROR;
  * MessageFormat *msgFmt = new MessageFormat(UnicodeString("{0} est  {1, select, female {allée} other {allé}} à Paris."), Locale("fr"),  status);
  * if (U_FAILURE(status)) {
  *       return;
  * }
  * FieldPosition ignore(FieldPosition::DONT_CARE);
  * UnicodeString result;
  *
  * char* str1= "Kirti,female";
  * Formattable args1[] = {"Kirti","female"};
  * msgFmt->format(args1, 2, result, ignore, status);
  * cout << "Input is " << str1 << " and result is: " << result << endl;
  * delete msgFmt;
  *
  * </pre>
  * Produces the output:<br/>
  * <code>Input is Kirti,female and result is: Kirti est allée à  Paris.</code>
  *
  * @draft ICU 4.4
  */

class U_I18N_API SelectFormat : public Format {
public:

    /**
     * Creates a new <code>SelectFormat</code> .
     * @param status  output param set to success/failure code on exit, which
     *                must not indicate a failure before the function call.
     * @draft ICU 4.4
     */
    SelectFormat(UErrorCode& status);

    /**
     * Creates a new <code>SelectFormat</code> for a given pattern string.
     * @param  pattern the pattern for this <code>SelectFormat</code>.
     *                 errors are returned to status if the pattern is invalid.
     * @param status   output param set to success/failure code on exit, which
     *                 must not indicate a failure before the function call.
     * @draft ICU 4.4
     */
    SelectFormat(const UnicodeString& pattern, UErrorCode& status);

    /**
     * copy constructor.
     * @draft ICU 4.4
     */
    SelectFormat(const SelectFormat& other);

    /**
     * Destructor.
     * @draft ICU 4.4
     */
    virtual ~SelectFormat();

    /**
     * Sets the pattern used by this select format.
     * for the keyword rules.
     * Patterns and their interpretation are specified in the class description.
     *
     * @param pattern the pattern for this select format
     *                errors are returned to status if the pattern is invalid.
     * @param status  output param set to success/failure code on exit, which
     *                must not indicate a failure before the function call.
     * @draft ICU 4.4
     */
    void applyPattern(const UnicodeString& pattern, UErrorCode& status);

    /**
     * Selects the phrase for  the given keyword
     *
     * @param keyword   a string (UnicodeString) The keyword for which to  
     *                 select a phrase . 
     * @param appendTo output parameter to receive result.
     *                 result is appended to existing contents.
     * @param success  output param set to success/failure code on exit, which
     *                 must not indicate a failure before the function call.
     * @return         Reference to 'appendTo' parameter. 
     * @draft ICU 4.4
     */
    UnicodeString& format(UnicodeString keyword, UnicodeString& appendTo,UErrorCode& success) const;

    /**
     * Selects the phrase for  the given keyword
     *
     * @param keyword   a string (UnicodeString) The keyword for which to  
     *                 select a phrase. 
     * @param appendTo output parameter to receive result.
     *                 result is appended to existing contents.
     * @param pos      On input: an alignment field, if desired.
     *                 On output: the offsets of the alignment field.
     * @param success  output param set to success/failure code on exit, which
     *                 must not indicate a failure before the function call.
     * @return         Reference to 'appendTo' parameter. 
     * @draft ICU 4.4
     */
    UnicodeString& format(UnicodeString keyword,
                            UnicodeString& appendTo,
                            FieldPosition& pos,
                            UErrorCode& success) const;

    /**
     * Assignment operator
     *
     * @param other    the SelectFormat object to copy from.
     * @draft ICU 4.4
     */
    SelectFormat& operator=(const SelectFormat& other);

    /**
     * Return true if another object is semantically equal to this one.
     *
     * @param other    the SelectFormat object to be compared with.
     * @return         true if other is semantically equal to this.
     * @draft ICU 4.4
     */
    virtual UBool operator==(const Format& other) const;

    /**
     * Return true if another object is semantically unequal to this one.
     *
     * @param other    the SelectFormat object to be compared with.
     * @return         true if other is semantically unequal to this.
     * @draft ICU 4.4
     */
    virtual UBool operator!=(const Format& other) const;

    /**
     * Clones this Format object polymorphically.  The caller owns the
     * result and should delete it when done.
     * @draft ICU 4.4
     */
    virtual Format* clone(void) const;

    /**
     * Redeclared Format method.
     *
     * @param obj       The object to be formatted into a string.
     * @param appendTo  output parameter to receive result.
     *                  Result is appended to existing contents.
     * @param pos       On input: an alignment field, if desired.
     *                  On output: the offsets of the alignment field.
     * @param status    output param filled with success/failure status.
     * @return          Reference to 'appendTo' parameter.
     * @draft ICU 4.4
     */
   UnicodeString& format(const Formattable& obj,
                         UnicodeString& appendTo,
                         FieldPosition& pos,
                         UErrorCode& status) const;

   /**
    * Returns the pattern from applyPattern() or constructor.
    *
    * @param  appendTo  output parameter to receive result.
    *                  Result is appended to existing contents.
    * @return the UnicodeString with inserted pattern.
    * @draft ICU 4.4
    */
   UnicodeString& toPattern(UnicodeString& appendTo);

   /**
    * This method is not yet supported by <code>SelectFormat</code>.
    * <P>
    * Before calling, set parse_pos.index to the offset you want to start
    * parsing at in the source. After calling, parse_pos.index is the end of
    * the text you parsed. If error occurs, index is unchanged.
    * <P>
    * When parsing, leading whitespace is discarded (with a successful parse),
    * while trailing whitespace is left as is.
    * <P>
    * See Format::parseObject() for more.
    *
    * @param source    The string to be parsed into an object.
    * @param result    Formattable to be set to the parse result.
    *                  If parse fails, return contents are undefined.
    * @param parse_pos The position to start parsing at. Upon return
    *                  this param is set to the position after the
    *                  last character successfully parsed. If the
    *                  source is not parsed successfully, this param
    *                  will remain unchanged.
    * @draft ICU 4.4
    */
   virtual void parseObject(const UnicodeString& source,
                            Formattable& result,
                            ParsePosition& parse_pos) const;

    /**
     * ICU "poor man's RTTI", returns a UClassID for this class.
     */
    static UClassID U_EXPORT2 getStaticClassID(void);

    /**
     * ICU "poor man's RTTI", returns a UClassID for the actual class.
     * @draft ICU 4.4
     */
     virtual UClassID getDynamicClassID() const;

private:
    typedef enum fmtToken {
        none,
        tLetter,
        tNumber,
        tSpace,
        tLeftBrace,
        tRightBrace
    }fmtToken;

    UnicodeString pattern;
    Hashtable  *parsedValuesHash;

    SelectFormat();   // default constructor not implemented
    void init(UErrorCode& status);
    UBool inRange(UChar ch, fmtToken& type);
    UBool checkSufficientDefinition();
    void parsingFailure();
    
    UnicodeString insertFormattedSelect( UnicodeString& message,
                                        UnicodeString& appendTo
                                        ) const;

    void copyHashtable(Hashtable *other, UErrorCode& status);
};

U_NAMESPACE_END

#endif /* #if !UCONFIG_NO_FORMATTING */

#endif // _SELFMT
//eof
