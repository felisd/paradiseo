// -*- mode: c++; c-indent-level: 4; c++-member-init-indent: 8; comment-column: 35; -*-

//-----------------------------------------------------------------------------
// eoParam.h
// (c) Marc Schoenauer, Maarten Keijzer and GeNeura Team, 2000
/* 
    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with this library; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

    Contact: todos@geneura.ugr.es, http://geneura.ugr.es
             Marc.Schoenauer@polytechnique.fr
             mkeijzer@dhi.dk
 */
//-----------------------------------------------------------------------------

#ifndef eoParam_h
#define eoParam_h

//-----------------------------------------------------------------------------
#include <math.h>		   // for floor
#include <string>
#include <strstream>
#include <vector>
#include <eoScalarFitness.h>   // for specializations
/**
    eoParam: Base class for monitoring and parsing parameters
*/
class eoParam 
{
public:
  
  /** Empty constructor - called from outside any parser
   */
  eoParam ()
    : repLongName(""), repDefault(""), repDescription(""), 
    repShortHand(0), repRequired(false){}

  /**
   * Construct a Param.
   * @param _longName      Long name of the argument
   * @param _default       The default value
   * @param _description   Description of the parameter. What is useful for.
   * @param _shortName     Short name of the argument (Optional)
   * @param _required      If it is a necessary parameter or not
   */
  eoParam (std::string _longName, std::string _default, 
      std::string _description, char _shortName = 0, bool _required = false)
    : repLongName(_longName), repDefault(_default),
    repDescription(_description ), 
    repShortHand(_shortName), repRequired( _required) {}
  
  /**
   * Virtual destructor is needed.
   */
  virtual ~eoParam () {};
  
  /**
  * Pure virtual function to get the value out. 
  */
  virtual std::string getValue ( void ) const = 0;

  /**
  * Pure virtual function to set the value
  */
  virtual void setValue(std::string _value)   = 0 ; 

  /**
   * Returns the short name.
   */
  char shortName ( void ) const { return repShortHand; };
  
  /**
   * Returns the long name.
   */
  const std::string& longName ( void ) const { return repLongName; };
  
  /**
   * Returns the description of the argument
   */
  const std::string& description ( void ) const { return repDescription; };
  
  /**
   * Returns the default value of the argument
   */
  const std::string& defValue ( void ) const { return repDefault; };
  
  /**
   * Sets the default value of the argument, 
   */
  void defValue ( std::string str ) { repDefault = str; };
  
  /**
   * Returns the value of the param as a string
   */
  /**
   * Returns if required or not.
   */
  bool required ( void ) const { return repRequired; };
   
private:
    std::string repLongName;
    std::string repDefault;
    std::string repDescription;
  char   repShortHand;
  bool repRequired;
};

/**
    eoValueParam<ValueType>: templatized derivation of eoParam. Can be used to contain 
    any scalar value type. It makes use of std::strstream to get and set values. This
    should be changed to std::stringstream when that class is available in g++.

    Note also that there is a template specialization for pair<double, double> and
    for vector<double>. These stream their contents delimited with whitespace.
*/

template <class ValueType>
class eoValueParam : public eoParam
{
public :
    eoValueParam(void) : eoParam() {}

  /**
   * Construct a Param.
   * @param _defaultValue       The default value
   * @param _longName           Long name of the argument
   * @param _description        Description of the parameter. What is useful for.
   * @param _shortName          Short name of the argument (Optional)
   * @param _required           If it is a necessary parameter or not
   */
    eoValueParam (ValueType _defaultValue,
                std::string _longName,
                std::string _description = "No description",
                char _shortHand = 0,
                bool _required = false)
    : eoParam(_longName, "", _description, _shortHand, _required), repValue(_defaultValue)
    {
        eoParam::defValue(getValue());
    }

    ValueType& value()              { return repValue; }
    ValueType  value() const        { return repValue; }

    std::string getValue(void) const
    {
        char buf[1024];
        std::ostrstream os(buf, 1023);
        os << repValue;
        os << std::ends;
        return os.str();
    }

    void setValue(std::string _value)
    {
        std::istrstream is(_value.c_str());
        is >> repValue;
    }

private :
    ValueType repValue;
};

/*
  Specialization for string
*/
template <>
std::string eoValueParam<std::string>::getValue(void) const
{
  return repValue;
}


template <>
void eoValueParam<bool>::setValue(std::string _value)
{
    if (_value.empty())
    {
        repValue = true;
        return;
    }

    std::istrstream is(_value.c_str());
    is >> repValue;
}


/// Because MSVC does not support partial specialization, the pair is a double, not a T
template <>
std::string eoValueParam<std::pair<double, double> >::getValue(void) const
{
    // use own buffer as MSVC's buffer leaks!
    char buff[1024];
    std::ostrstream os(buff, 1024);
    os << repValue.first << ' ' << repValue.second << std::ends;
    return os.str();
}

/// Because MSVC does not support partial specialization, the pair is a double, not a T
template <>
void eoValueParam<std::pair<double, double> >::setValue(std::string _value)
{
    std::istrstream is(_value.c_str());
    is >> repValue.first;
    is >> repValue.second;
}

/// Because MSVC does not support partial specialization, the vector is a vector of doubles, not a T
template <>
std::string eoValueParam<std::vector<std::vector<double> > >::getValue(void) const
{
    std::ostrstream os;
    os << repValue.size() << ' ';
    for (unsigned i = 0; i < repValue.size(); ++i)
    {
      os << repValue[i].size() << ' ';
      std::copy(repValue[i].begin(), repValue[i].end(), std::ostream_iterator<double>(os, " "));
    }

    os << std::ends;
    return os.str();
}

/// Because MSVC does not support partial specialization, the vector is a vector of doubles, not a T
template <>
void eoValueParam<std::vector<std::vector<double> > >::setValue(std::string _value)
{
    std::istrstream is(_value.c_str());
    unsigned sz;
    is >> sz;
    repValue.resize(sz);

    for (unsigned i = 0; i < repValue.size(); ++i)
    {
      unsigned sz2;
      is >> sz2;
      repValue[i].resize(sz2);
      for (unsigned j = 0; j < sz2; ++j)
      {
        is >> repValue[i][j];
      }
    }
}

/// Because MSVC does not support partial specialization, the vector is a double, not a T
template <>
std::string eoValueParam<std::vector<double> >::getValue(void) const
{
    std::ostrstream os;
    os << repValue.size() << ' ';
    std::copy(repValue.begin(), repValue.end(), std::ostream_iterator<double>(os, " "));
    os << std::ends;
    return os.str();
}

/// Because MSVC does not support partial specialization, the vector is a double, not a T
template <>
void eoValueParam<std::vector<double> >::setValue(std::string _value)
{
    std::istrstream is(_value.c_str());
    unsigned sz;
    is >> sz;
    repValue.resize(sz);
    std::copy(std::istream_iterator<double>(is), std::istream_iterator<double>(), repValue.begin());
}

/// Because MSVC does not support partial specialization, the vector is a eoMinimizingFitness, not a T
template <>
std::string eoValueParam<std::vector<eoMinimizingFitness> >::getValue(void) const
{
    std::ostrstream os;
    os << repValue.size() << ' ';
    std::copy(repValue.begin(), repValue.end(), std::ostream_iterator<eoMinimizingFitness>(os, " "));
    os << std::ends;
    return os.str();
}

/// Because MSVC does not support partial specialization, the vector is a eoMinimizingFitness, not a T
// NOTE: g++ doesn support it either!!!
template <>
void eoValueParam<std::vector<eoMinimizingFitness> >::setValue(std::string _value)
{
    std::istrstream is(_value.c_str());
    unsigned sz;
    is >> sz;
    repValue.resize(sz);
    std::copy(std::istream_iterator<eoMinimizingFitness>(is), std::istream_iterator<eoMinimizingFitness>(), repValue.begin());
}

/*   ABANDONNED - See class eoRateType below
     ///////////////////////////////////////
  Specialization for "rate_or_absolute-number"

typedef std::pair<double,bool> eoRateType;
template <>
std::string eoValueParam<eoRateType>::getValue(void) const
{
    std::ostrstream os;
    if (repValue.second)
      os << 100*repValue.first << "% " << std::ends;
    else
      os << repValue.first << " " << std::ends;
    return os.str();
}

template <>
void eoValueParam<eoRateType>::setValue(std::string _value)
{
  double rate;
  // check for %
  bool interpret_as_rate = false;   // == no %
  size_t pos =  _value.find('%');
  if (pos < _value.size())  //  found a %
    {
      interpret_as_rate = true;
      _value.resize(pos);	   // get rid of %
    }
    std::istrstream is(_value.c_str());
    is >> rate;
    repValue.first = (interpret_as_rate ? rate/100 : floor(rate));
    repValue.second = interpret_as_rate;
}
*/
/** 
 * A helper class for the parsing of parameters that can be either a rate
 * or an absolute value (integer)
 * See eoHowMany.h 
 */
class eoRateParamType : public std::pair<double,bool>
{
public:
  eoRateParamType(std::pair<double,bool> _p) : std::pair<double,bool>(_p) {}
  eoRateParamType(std::string _value)
  {
    readFrom(_value);
  }

  ostream & printOn(ostream & _os) const
  {
    if (second)
      _os << 100*first << "% " << std::ends;
    else
      _os << first << " " << std::ends;
    return _os;
  }

  istream & readFrom(istream & _is)
  {
    string value;
    _is >> value;
    readFrom(value);
    return _is;
  }

  void readFrom(std::string _value)
  {
    double rate;
    // check for %
    bool interpret_as_rate = false;   // == no %
    size_t pos =  _value.find('%');
    if (pos < _value.size())  //  found a %
      {
	interpret_as_rate = true;
	_value.resize(pos);	   // get rid of %
      }
    std::istrstream is(_value.c_str());
    is >> rate;
    first = (interpret_as_rate ? rate/100 : floor(rate));
    second = interpret_as_rate;
  }


};

// at the moment, the following are defined in eoParser.cpp
ostream & operator<<(ostream & _os, const eoRateParamType & _rate);
istream & operator>>(istream & _is,  eoRateParamType & _rate);



/*template <class ContainerType>
class eoContainerParam : public eoParam
{
public :
    eoContainerParam (ContainerType& value, string _shortName, string _longName,
	            string _default,
	            string _description,
                bool _required,
	            bool _change )
    : value(_value), eoParam(_shortName, _longName, _description, _default, _required, _change)
    {}


  //  void setValue(const string & _value)
   // {
   //     std::istringstream is(_value);
    //    copy(istream_iterator<Container::value_type>(is), istream_iterator<Container::value_type>(), back_inserter(value));
   // }

private :
    ContainerType& value;
};*/

/** 
 * Another helper class for parsing parameters like
 * Keyword(arg1, arg2, ...)
 *
 * It is basically a pair<string,vector<string> >
 *       first string is keyword
 *       the vector<string> contains all arguments (as strings)
 * See make_algo.h
 */

class eoParamParamType : public std::pair<string,std::vector<string> >
{
public:
  eoParamParamType(std::string _value)
  {
    readFrom(_value);
  }

  ostream & printOn(ostream & _os) const
  {
    _os << first;
    unsigned narg = second.size();
    if (!narg)
      return _os;

    // Here, we do have args
    _os << "(";
    if (narg == 1)	   // 1 arg only
      {
	_os << second[0] << ")" ;
	return _os;
      }
    // and here more than 1 arg
    for (unsigned i=0; i<narg-1; i++)
      _os << second[i] << "," ;
    _os << second[narg-1] << ")";
    return _os;
  }

  istream & readFrom(istream & _is)
  {
    string value;
    _is >> value;
    readFrom(value);
    return _is;
  }

  void readFrom(std::string &  _value)
  {
    second.resize(0);		   // just in case
    size_t pos = _value.find('(');
    if (pos >= _value.size())	   // no arguments
      {
	first = _value;
	return;
      }
    // so here we do have arguments
    string t = _value.substr(pos+1);// the arguments
    _value.resize(pos);
    first = _value;    // done for the keyword (NOTE: may be empty string!)

    // now all arguments    
  string delim(" (),");
  while ( (pos=t.find_first_not_of(delim)) < t.size())
    {
      size_t posEnd = t.find_first_of(delim, pos);
      string u(t,pos);
      u.resize(posEnd-pos);
      second.push_back(u);
      t = t.substr(posEnd);
    }
  }
};

// at the moment, the following are defined in eoParser.cpp
ostream & operator<<(ostream & _os, const eoParamParamType & _rate);
istream & operator>>(istream & _is,  eoParamParamType & _rate);

#endif
