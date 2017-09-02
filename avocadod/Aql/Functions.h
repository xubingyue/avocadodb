////////////////////////////////////////////////////////////////////////////////
/// DISCLAIMER
///
/// Copyright 2014-2016 AvocadoDB GmbH, Cologne, Germany
/// Copyright 2004-2014 triAGENS GmbH, Cologne, Germany
///
/// Licensed under the Apache License, Version 2.0 (the "License");
/// you may not use this file except in compliance with the License.
/// You may obtain a copy of the License at
///
///     http://www.apache.org/licenses/LICENSE-2.0
///
/// Unless required by applicable law or agreed to in writing, software
/// distributed under the License is distributed on an "AS IS" BASIS,
/// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
/// See the License for the specific language governing permissions and
/// limitations under the License.
///
/// Copyright holder is AvocadoDB GmbH, Cologne, Germany
///
/// @author Jan Steemann
////////////////////////////////////////////////////////////////////////////////

#ifndef AVOCADOD_AQL_FUNCTIONS_H
#define AVOCADOD_AQL_FUNCTIONS_H 1

#include "Basics/Common.h"
#include "Basics/SmallVector.h"
#include "Aql/AqlValue.h"

namespace avocadodb {
namespace transaction {
class Methods;
}

namespace basics {
class VPackStringBufferAdapter;
}

namespace aql {

class Query;

typedef std::function<bool()> ExecutionCondition;

typedef SmallVector<AqlValue> VPackFunctionParameters;

typedef std::function<AqlValue(avocadodb::aql::Query*, transaction::Methods*,
                                VPackFunctionParameters const&)>
    FunctionImplementation;

struct Functions {
  protected:

  /// @brief validate the number of parameters
   static void ValidateParameters(VPackFunctionParameters const& parameters,
                                  char const* function, int minParams,
                                  int maxParams);

   static void ValidateParameters(VPackFunctionParameters const& parameters,
                                  char const* function, int minParams);

   /// @brief extract a function parameter from the arguments
   static AqlValue ExtractFunctionParameterValue(
       transaction::Methods*, VPackFunctionParameters const& parameters,
       size_t position);

   /// @brief extra a collection name from an AqlValue
   static std::string ExtractCollectionName(
       transaction::Methods* trx, VPackFunctionParameters const& parameters,
       size_t position);

   /// @brief extract attribute names from the arguments
   static void ExtractKeys(std::unordered_set<std::string>& names,
                           avocadodb::aql::Query* query,
                           transaction::Methods* trx,
                           VPackFunctionParameters const& parameters,
                           size_t startParameter, char const* functionName);

   /// @brief Helper function to merge given parameters
   ///        Works for an array of objects as first parameter or arbitrary many
   ///        object parameters
   static AqlValue MergeParameters(avocadodb::aql::Query* query,
                                   transaction::Methods* trx,
                                   VPackFunctionParameters const& parameters,
                                   char const* funcName, bool recursive);

  public:
   /// @brief called before a query starts
   /// has the chance to set up any thread-local storage
   static void InitializeThreadContext();

   /// @brief called when a query ends
   /// its responsibility is to clear any thread-local storage
   static void DestroyThreadContext();

   /// @brief helper function. not callable as a "normal" AQL function
   static void Stringify(transaction::Methods* trx,
                         avocadodb::basics::VPackStringBufferAdapter& buffer,
                         VPackSlice const& slice);

   static AqlValue IsNull(avocadodb::aql::Query*, transaction::Methods*,
                          VPackFunctionParameters const&);
   static AqlValue IsBool(avocadodb::aql::Query*, transaction::Methods*,
                          VPackFunctionParameters const&);
   static AqlValue IsNumber(avocadodb::aql::Query*, transaction::Methods*,
                            VPackFunctionParameters const&);
   static AqlValue IsString(avocadodb::aql::Query*, transaction::Methods*,
                            VPackFunctionParameters const&);
   static AqlValue IsArray(avocadodb::aql::Query*, transaction::Methods*,
                           VPackFunctionParameters const&);
   static AqlValue IsObject(avocadodb::aql::Query*, transaction::Methods*,
                            VPackFunctionParameters const&);
   static AqlValue Typename(avocadodb::aql::Query*, transaction::Methods*,
                            VPackFunctionParameters const&);
   static AqlValue ToNumber(avocadodb::aql::Query*, transaction::Methods*,
                            VPackFunctionParameters const&);
   static AqlValue ToString(avocadodb::aql::Query*, transaction::Methods*,
                            VPackFunctionParameters const&);
   static AqlValue ToBool(avocadodb::aql::Query*, transaction::Methods*,
                          VPackFunctionParameters const&);
   static AqlValue ToArray(avocadodb::aql::Query*, transaction::Methods*,
                           VPackFunctionParameters const&);
   static AqlValue Length(avocadodb::aql::Query*, transaction::Methods*,
                          VPackFunctionParameters const&);
   static AqlValue First(avocadodb::aql::Query*, transaction::Methods*,
                         VPackFunctionParameters const&);
   static AqlValue Last(avocadodb::aql::Query*, transaction::Methods*,
                        VPackFunctionParameters const&);
   static AqlValue Nth(avocadodb::aql::Query*, transaction::Methods*,
                       VPackFunctionParameters const&);
   static AqlValue Contains(avocadodb::aql::Query*, transaction::Methods*,
                            VPackFunctionParameters const&);
   static AqlValue Concat(avocadodb::aql::Query*, transaction::Methods*,
                          VPackFunctionParameters const&);
   static AqlValue ConcatSeparator(avocadodb::aql::Query*,
                                   transaction::Methods*,
                                   VPackFunctionParameters const&);
   static AqlValue CharLength(avocadodb::aql::Query*,
                              transaction::Methods*,
                              VPackFunctionParameters const&);
   static AqlValue Lower(avocadodb::aql::Query*,
                              transaction::Methods*,
                              VPackFunctionParameters const&);
   static AqlValue Upper(avocadodb::aql::Query*,
                              transaction::Methods*,
                              VPackFunctionParameters const&);
   static AqlValue Like(avocadodb::aql::Query*, transaction::Methods*,
                        VPackFunctionParameters const&);
   static AqlValue RegexTest(avocadodb::aql::Query*, transaction::Methods*,
                             VPackFunctionParameters const&);
   static AqlValue RegexReplace(avocadodb::aql::Query*, transaction::Methods*,
                                VPackFunctionParameters const&);
   static AqlValue Passthru(avocadodb::aql::Query*, transaction::Methods*,
                            VPackFunctionParameters const&);
   static AqlValue Unset(avocadodb::aql::Query*, transaction::Methods*,
                         VPackFunctionParameters const&);
   static AqlValue UnsetRecursive(avocadodb::aql::Query*, transaction::Methods*,
                                  VPackFunctionParameters const&);
   static AqlValue Keep(avocadodb::aql::Query*, transaction::Methods*,
                        VPackFunctionParameters const&);
   static AqlValue Merge(avocadodb::aql::Query*, transaction::Methods*,
                         VPackFunctionParameters const&);
   static AqlValue MergeRecursive(avocadodb::aql::Query*, transaction::Methods*,
                                  VPackFunctionParameters const&);
   static AqlValue Has(avocadodb::aql::Query*, transaction::Methods*,
                       VPackFunctionParameters const&);
   static AqlValue Attributes(avocadodb::aql::Query*, transaction::Methods*,
                              VPackFunctionParameters const&);
   static AqlValue Values(avocadodb::aql::Query*, transaction::Methods*,
                          VPackFunctionParameters const&);
   static AqlValue Min(avocadodb::aql::Query*, transaction::Methods*,
                       VPackFunctionParameters const&);
   static AqlValue Max(avocadodb::aql::Query*, transaction::Methods*,
                       VPackFunctionParameters const&);
   static AqlValue Sum(avocadodb::aql::Query*, transaction::Methods*,
                       VPackFunctionParameters const&);
   static AqlValue Average(avocadodb::aql::Query*, transaction::Methods*,
                           VPackFunctionParameters const&);
    static AqlValue Sleep(avocadodb::aql::Query*, transaction::Methods*,
                          VPackFunctionParameters const&);
   static AqlValue RandomToken(avocadodb::aql::Query*, transaction::Methods*,
                               VPackFunctionParameters const&);
   static AqlValue Md5(avocadodb::aql::Query*, transaction::Methods*,
                       VPackFunctionParameters const&);
   static AqlValue Sha1(avocadodb::aql::Query*, transaction::Methods*,
                        VPackFunctionParameters const&);
   static AqlValue Hash(avocadodb::aql::Query*, transaction::Methods*,
                        VPackFunctionParameters const&);
   static AqlValue Unique(avocadodb::aql::Query*, transaction::Methods*,
                          VPackFunctionParameters const&);
   static AqlValue SortedUnique(avocadodb::aql::Query*, transaction::Methods*,
                                VPackFunctionParameters const&);
   static AqlValue Union(avocadodb::aql::Query*, transaction::Methods*,
                         VPackFunctionParameters const&);
   static AqlValue UnionDistinct(avocadodb::aql::Query*, transaction::Methods*,
                                 VPackFunctionParameters const&);
   static AqlValue Intersection(avocadodb::aql::Query*, transaction::Methods*,
                                VPackFunctionParameters const&);
   static AqlValue Outersection(avocadodb::aql::Query*, transaction::Methods*,
                                VPackFunctionParameters const&);
   static AqlValue Distance(avocadodb::aql::Query*, transaction::Methods*,
                            VPackFunctionParameters const&);
   static AqlValue Flatten(avocadodb::aql::Query*, transaction::Methods*,
                           VPackFunctionParameters const&);
   static AqlValue Zip(avocadodb::aql::Query*, transaction::Methods*,
                       VPackFunctionParameters const&);
   static AqlValue JsonStringify(avocadodb::aql::Query*, transaction::Methods*,
                                 VPackFunctionParameters const&);
   static AqlValue JsonParse(avocadodb::aql::Query*, transaction::Methods*,
                             VPackFunctionParameters const&);
   static AqlValue ParseIdentifier(avocadodb::aql::Query*,
                                   transaction::Methods*,
                                   VPackFunctionParameters const&);
   static AqlValue Slice(avocadodb::aql::Query*, transaction::Methods*,
                         VPackFunctionParameters const&);
   static AqlValue Minus(avocadodb::aql::Query*, transaction::Methods*,
                         VPackFunctionParameters const&);
   static AqlValue Document(avocadodb::aql::Query*, transaction::Methods*,
                            VPackFunctionParameters const&);
   static AqlValue Round(avocadodb::aql::Query*, transaction::Methods*,
                         VPackFunctionParameters const&);
   static AqlValue Abs(avocadodb::aql::Query*, transaction::Methods*,
                       VPackFunctionParameters const&);
   static AqlValue Ceil(avocadodb::aql::Query*, transaction::Methods*,
                        VPackFunctionParameters const&);
   static AqlValue Floor(avocadodb::aql::Query*, transaction::Methods*,
                         VPackFunctionParameters const&);
   static AqlValue Sqrt(avocadodb::aql::Query*, transaction::Methods*,
                        VPackFunctionParameters const&);
   static AqlValue Pow(avocadodb::aql::Query*, transaction::Methods*,
                       VPackFunctionParameters const&);
   static AqlValue Log(avocadodb::aql::Query*, transaction::Methods*,
                       VPackFunctionParameters const&);
   static AqlValue Log2(avocadodb::aql::Query*, transaction::Methods*,
                        VPackFunctionParameters const&);
   static AqlValue Log10(avocadodb::aql::Query*, transaction::Methods*,
                         VPackFunctionParameters const&);
   static AqlValue Exp(avocadodb::aql::Query*, transaction::Methods*,
                       VPackFunctionParameters const&);
   static AqlValue Exp2(avocadodb::aql::Query*, transaction::Methods*,
                        VPackFunctionParameters const&);
   static AqlValue Sin(avocadodb::aql::Query*, transaction::Methods*,
                       VPackFunctionParameters const&);
   static AqlValue Cos(avocadodb::aql::Query*, transaction::Methods*,
                       VPackFunctionParameters const&);
   static AqlValue Tan(avocadodb::aql::Query*, transaction::Methods*,
                       VPackFunctionParameters const&);
   static AqlValue Asin(avocadodb::aql::Query*, transaction::Methods*,
                        VPackFunctionParameters const&);
   static AqlValue Acos(avocadodb::aql::Query*, transaction::Methods*,
                        VPackFunctionParameters const&);
   static AqlValue Atan(avocadodb::aql::Query*, transaction::Methods*,
                        VPackFunctionParameters const&);
   static AqlValue Atan2(avocadodb::aql::Query*, transaction::Methods*,
                         VPackFunctionParameters const&);
   static AqlValue Radians(avocadodb::aql::Query*, transaction::Methods*,
                           VPackFunctionParameters const&);
   static AqlValue Degrees(avocadodb::aql::Query*, transaction::Methods*,
                           VPackFunctionParameters const&);
   static AqlValue Pi(avocadodb::aql::Query*, transaction::Methods*,
                      VPackFunctionParameters const&);
   static AqlValue Rand(avocadodb::aql::Query*, transaction::Methods*,
                        VPackFunctionParameters const&);
   static AqlValue FirstDocument(avocadodb::aql::Query*, transaction::Methods*,
                                 VPackFunctionParameters const&);
   static AqlValue FirstList(avocadodb::aql::Query*, transaction::Methods*,
                             VPackFunctionParameters const&);
   static AqlValue Push(avocadodb::aql::Query*, transaction::Methods*,
                        VPackFunctionParameters const&);
   static AqlValue Pop(avocadodb::aql::Query*, transaction::Methods*,
                       VPackFunctionParameters const&);
   static AqlValue Append(avocadodb::aql::Query*, transaction::Methods*,
                          VPackFunctionParameters const&);
   static AqlValue Unshift(avocadodb::aql::Query*, transaction::Methods*,
                           VPackFunctionParameters const&);
   static AqlValue Shift(avocadodb::aql::Query*, transaction::Methods*,
                         VPackFunctionParameters const&);
   static AqlValue RemoveValue(avocadodb::aql::Query*, transaction::Methods*,
                               VPackFunctionParameters const&);
   static AqlValue RemoveValues(avocadodb::aql::Query*, transaction::Methods*,
                                VPackFunctionParameters const&);
   static AqlValue RemoveNth(avocadodb::aql::Query*, transaction::Methods*,
                             VPackFunctionParameters const&);
   static AqlValue NotNull(avocadodb::aql::Query*, transaction::Methods*,
                           VPackFunctionParameters const&);
   static AqlValue CurrentDatabase(avocadodb::aql::Query*,
                                   transaction::Methods*,
                                   VPackFunctionParameters const&);
   static AqlValue CollectionCount(avocadodb::aql::Query*,
                                   transaction::Methods*,
                                   VPackFunctionParameters const&);
   static AqlValue VarianceSample(avocadodb::aql::Query*, transaction::Methods*,
                                  VPackFunctionParameters const&);
   static AqlValue PregelResult(avocadodb::aql::Query*, transaction::Methods*,
                                VPackFunctionParameters const&);
   static AqlValue VariancePopulation(avocadodb::aql::Query*,
                                      transaction::Methods*,
                                      VPackFunctionParameters const&);
   static AqlValue StdDevSample(avocadodb::aql::Query*, transaction::Methods*,
                                VPackFunctionParameters const&);
   static AqlValue StdDevPopulation(avocadodb::aql::Query*,
                                    transaction::Methods*,
                                    VPackFunctionParameters const&);
   static AqlValue Median(avocadodb::aql::Query*, transaction::Methods*,
                          VPackFunctionParameters const&);
   static AqlValue Percentile(avocadodb::aql::Query*, transaction::Methods*,
                              VPackFunctionParameters const&);
   static AqlValue Range(avocadodb::aql::Query*, transaction::Methods*,
                         VPackFunctionParameters const&);
   static AqlValue Position(avocadodb::aql::Query*, transaction::Methods*,
                            VPackFunctionParameters const&);
   static AqlValue IsSameCollection(avocadodb::aql::Query*,
                                    transaction::Methods*,
                                    VPackFunctionParameters const&);
};
}
}

#endif
