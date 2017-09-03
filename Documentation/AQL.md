
# avocadodb-aql详细操作

------------------------------------------------

>下面介绍以下高级操作:

*  **FOR**:遍历数组的所有元素。

*  **RETURN**:生成查询的结果。

*  **FILTER**:将结果限制为与任意逻辑条件匹配的元素。

*  **SORT**:强制排序已生成的中间结果的数组。

*  **LIMIT**:将结果中的元素数减少到至多指定的数字, 可以选择跳过元素 (分页)。

*  **LET**:将任意值赋给变量。

*  **COLLECT**:按一个或多个组条件对数组进行分组。也可以计数和聚合。

*  **REMOVE**:从集合中移除文档。

*  **UPDATE**:部分更新集合中的文档。

*  **REPLACE**:完全替换集合中的文档。

*  **INSERT**:将新文档插入到集合中。

*  **UPSERT**:更新/替换现有文档, 或在不存在的情况下创建它。

*  **WITH**:指定查询中使用的集合 (仅在查询开始时)。


---------------------------------------------



#   FOR

>FOR 关键字可以是循环访问数组的所有元素。一般语法是:

```sql
FOR variableName IN expression

```

图遍历还有一个特殊的变体:

```
FOR vertexVariableName, edgeVariableName, pathVariableName IN traversalExpression

```

每个由表达式返回的数组元素仅访问一次。在所有情况下, 表达式都需要返回一个数组。也允许空数组。当前数组元素可用于在 variableName 指定的变量中进行进一步处理。

```
FOR u IN users
RETURN u

返回值

[
{
  "_key": "2427801",
  "_id": "ks/2427801",
  "_rev": "_VeWiZ2i---",
  "id": 1,
  "a": "test",
  "b": [
    "aaaaaaaaaaaaaaaaa"
  ]
}
]


```

这将遍历阵列用户的所有元素 (注意: 此数组由本例中名为 "users" 的集合中的所有文档组成), 并使当前数组元素在变量 u 中可用. 在本例中没有修改, 只是使用 RETURN 关键字推入结果。

注意: 当迭代基于数组时, 如下所示, 文档的顺序是未定义的, 除非使用排序语句定义了显式排序顺序。

FOR 引入的变量是可用的, 直到 FOR 所放置的范围关闭。

另一个使用静态声明的值数组循环访问的示例:

```
FOR year IN [ 2011, 2012, 2013 ]
RETURN { "year" : year, "isLeapYear" : year % 4 == 0 && (year % 100 != 0 || year % 400 == 0) }

```

也允许多个语句的嵌套。当对语句进行嵌套时, 将创建由单个语句返回的数组元素的交叉乘积。

```
FOR u IN users
FOR l IN locations
  RETURN { "user" : u, "location" : l }
```
在此示例中, 有两个数组迭代: 在数组用户上的外部迭代加上在数组位置上的内部迭代。内部数组的遍历次数与外部数组中的元素数相同。对于每个迭代, 用户和位置的当前值都可用于在变量中进行进一步的处理。


---------------------------------------------

#  RETURN

返回语句可用于生成查询结果。必须在数据选择查询的每个块的末尾指定 RETURN 语句, 否则查询结果将是未定义的。在数据修改查询中使用主级别的返回是可选的。

```
RETURN expression

```

返回语句所返回的表达式是在返回声明所放置的块中的每个迭代中生成的。这意味着返回语句的结果始终是一个数组。这包括一个空数组, 如果没有与查询匹配的文档, 则返回一个返回值作为数组的一个元素。

要在不修改的情况下返回当前迭代数组中的所有元素, 可以使用以下简单形式:

```
FOR variableName IN expression
RETURN variableName
```

当返回允许指定表达式时, 可以执行任意计算来计算结果元素。可将返回的范围中有效的任何变量用于计算。

若要循环访问名为 users 的集合的所有文档并返回完整文档, 可以编写:

```
FOR u IN users
RETURN u
```

在 for 循环的每个迭代中, 用户集合的文档被分配给一个变量, 并在本例中未修改返回。若要只返回每个文档的一个属性, 可以使用不同的返回表达式:

```
FOR u IN users
RETURN u.name
```

或者要返回多个属性, 可以像这样构造一个对象:

```
FOR u IN users
RETURN { name: u.name, age: u.age }

```

注意: 返回将关闭当前范围并消除其中的所有局部变量。在使用子查询时要记住这一点很重要。

```
FOR u IN users
RETURN { [ u._id ]: u.age }
```

在本示例中, 每个用户的文档 _id 用作表达式来计算属性键:

```
[
{
  "users/9883": 32
},
{
  "users/9915": 27
},
{
  "users/10074": 69
}
]
```

结果包含每个用户一个具有单个键/值对的对象。这通常是不需要的。对于将用户 id 映射到年龄的单个对象, 需要合并单个结果并返回另一个返回:

```
RETURN MERGE(
  FOR u IN users
    RETURN { [ u._id ]: u.age }
)
```

.

```
[
{
  "users/10074": 69,
  "users/9883": 32,
  "users/9915": 27
}
]
```

请记住, 如果键表达式多次计算为相同的值, 则只有其中一个具有重复名称的键/值对才能生存合并 ()。为了避免出现这种情况, 您可以不使用动态属性名, 而改用静态名称, 并将所有文档属性作为属性值返回:


```
FOR u IN users
RETURN { name: u.name, age: u.age }
```

.


```
[
{
  "name": "John Smith",
  "age": 32
},
{
  "name": "James Hendrix",
  "age": 69
},
{
  "name": "Katie Foster",
  "age": 27
}
]
```

-----------------------------------------------------

#   FILTER

>筛选语句可用于将结果限制为与任意逻辑条件匹配的元素。

常规语法

```
FILTER condition


```

条件必须是计算结果为 false 或 true 的条件。如果条件结果为 false, 则跳过当前元素, 因此不会进一步处理它, 也不会成为结果的一部分。如果条件为 true, 则不跳过当前元素, 并且可以进一步处理。有关可以在条件中使用的比较运算符、逻辑运算符等的列表, 请参见运算符。

```
FOR u IN users
FILTER u.active == true && u.age < 39
RETURN u
```

允许在查询中指定多个筛选语句, 即使在同一块中也是如此。如果使用了多个筛选器语句, 则它们的结果将与逻辑 and 合并, 这意味着所有筛选条件都必须为真, 才能包含元素。

```
FOR u IN users
FILTER u.active == true
FILTER u.age < 39
RETURN u
```


在上面的示例中, 用户的所有数组元素的值都为 true, 且属性的值小于 39 (包括 null), 将包括在结果中。将跳过所有其他用户元素, 而不会将其包含在返回结果中。您可以参考从集合访问数据的章节来描述不存在或 null 属性的影响。

#####  操作顺序

请注意, 筛选语句的位置可能会影响查询的结果。测试数据中有16活动用户, 例如:

```
FOR u IN users
FILTER u.active == true
RETURN u
```

我们最多可以将结果集限制为5用户:

```
FOR u IN users
FILTER u.active == true
LIMIT 5
RETURN u
```

这可能会返回的用户文件, 吉姆, 迭戈, 安东尼, 迈克尔和克洛伊的例子。返回的是未定义的, 因为没有用于确保特定顺序的排序语句。如果我们添加第二个筛选语句只返回女性..。

```
FOR u IN users
FILTER u.active == true
LIMIT 5
FILTER u.gender == "f"
RETURN u
```

它可能只返回克洛伊文档, 因为该限制在第二个筛选器之前应用。不超过5文件到达第二个过滤器块, 并且不是所有他们完成性别标准, 即使有超过5活跃女性用户在汇集。通过添加排序块可以实现更具确定性的结果:

```
FOR u IN users
FILTER u.active == true
SORT u.age ASC
LIMIT 5
FILTER u.gender == "f"
RETURN u
```

这将返回用户玛丽亚和玛丽。如果按年龄降序排序, 则返回索菲亚、艾玛和麦迪逊文件。但在限制之后的筛选不是很常见, 您可能需要这样的查询:

```
FOR u IN users
FILTER u.active == true AND u.gender == "f"
SORT u.age ASC
LIMIT 5
RETURN u
```

放置过滤块的意义在于, 这个单一的关键字可以担当两个 SQL 关键字的角色, 并且具有。因此, AQL 的过滤器与任何其他中间结果、文档属性等的收集聚合体相同。


----------------------------------------------

#  SORT

排序语句将强制在当前块中已生成的中间结果的数组排序。排序允许指定一个或多个排序条件和方向。一般语法是:

```
SORT expression direction

```

按姓氏排序的示例查询 (按升序排列), 然后是名字 (按升序排列), 然后按 id (按降序排列):

```
FOR u IN users
SORT u.lastName, u.firstName, u.id DESC
RETURN u
```

指定方向是可选的。排序表达式的默认 (隐式) 方向为升序顺序。若要显式指定排序方向, 可以使用关键字 ASC (升序) 和降序。可以使用逗号分隔多个排序条件。在这种情况下, 为每个表达式 sperately 指定方向。例如

```
SORT doc.lastName, doc.firstName

```

将首先按姓氏以升序排序文档, 然后按名字以升序排列。

```
SORT doc.lastName DESC, doc.firstName

```
将首先按姓氏按降序排列文档, 然后按名字以升序排序。

```
SORT doc.lastName, doc.firstName DESC

```

将首先按姓氏以升序排序文档, 然后按名字降序排列。

注意: 当迭代基于数组时, 文档的顺序始终是未定义的, 除非使用排序定义了显式排序顺序。

请注意, 常量排序表达式可用于指示不需要特定的排序顺序。在优化过程中, AQL 优化器将对常量排序表达式进行优化, 但如果优化器不需要考虑任何特定的排序顺序, 则显式指定它们可能会启用进一步优化。这在收集语句之后尤其如此, 它应该产生一个排序结果。在收集语句后指定额外的排序空值允许 AQL 优化器完全删除收集结果的 post-sorting。


----------------------------------------------------------

#   LIMIT

>限制语句允许使用偏移量和计数对结果数组进行切片。它将结果中的元素数减少到最多指定的数字。采用了两种一般的限制形式:

```
LIMIT count
LIMIT offset, count
```

第一个窗体允许只指定计数值, 而第二个窗体允许指定偏移量和计数。第一个窗体是相同的, 使用第二个窗体的偏移值为0。

```
FOR u IN users
LIMIT 5
RETURN u
```

上面的查询返回用户集合的前五文档。它也可以写为限制 0, 5 为相同的结果。它实际上返回的文件是相当任意的, 因为没有明确的排序顺序被指定然而。因此, 限制应通常伴随排序操作。

偏移值指定应跳过结果中的多少元素。它必须是0或更大。count 值指定在结果中最多包含多少元素

```
FOR u IN users
SORT u.firstName, u.lastName, u.id DESC
LIMIT 2, 5
RETURN u
```

在上面的示例中, 对用户的文档进行排序, 前两个结果被跳过, 并返回下一个五用户文档。

请注意, 变量和表达式不能用于偏移和计数。在查询编译时, 它们的值必须是已知的, 这意味着您只能使用数字文本和绑定参数。

在与查询中的其他操作相关的情况下, 使用限制是有意义的。特别是在筛选器之前限制操作可以显著地更改结果, 因为这些操作是按照它们在查询中的写入顺序执行的。有关详细示例, 请参见筛选器。

------------------------------------------------

#   LET

>"LET" 语句可用于将任意值赋给变量。然后在让语句所放置的范围中引入变量。


```
LET variableName = expression

```

变量在 AQL 中是不可变的, 这意味着它们不能重新:

```
LET a = [1, 2, 3]  // initial assignment

a = PUSH(a, 4)     // syntax error, unexpected identifier
LET a = PUSH(a, 4) // parsing error, variable 'a' is assigned multiple times
LET b = PUSH(a, 4) // allowed, result: [1, 2, 3, 4]
```

让语句主要用于声明复杂计算, 并避免在查询的多个部分重复计算相同的值。

```
FOR u IN users
LET numRecommendations = LENGTH(u.recommendations)
RETURN {
  "user" : u,
  "numRecommendations" : numRecommendations,
  "isPowerUser" : numRecommendations >= 10
}
```

在上面的示例中, 使用 "LET" 语句计算出建议的数量, 从而避免在 RETURN 语句中计算两次值。

让我们使用的另一个用例是在子查询中声明一个复杂的计算, 使整个查询更具可读性。

```

FOR u IN users
LET friends = (
FOR f IN friends
  FILTER u.id == f.userId
  RETURN f
)
LET memberships = (
FOR m IN memberships
  FILTER u.id == m.userId
    RETURN m
)
RETURN {
  "user" : u,
  "friends" : friends,
  "numFriends" : LENGTH(friends),
  "memberShips" : memberships
}
```

------------------------------------------------

#   COLLECT


>"COLLECT" 关键字可用于按一个或多个组条件对数组进行分组。

COLLECT语句将消除当前范围内的所有局部变量。COLLECT后, 只有由COLLECT本身引入的变量是可用的。

COLLECT的一般语法是:

```
COLLECT variableName = expression options
COLLECT variableName = expression INTO groupsVariable options
COLLECT variableName = expression INTO groupsVariable = projectionExpression options
COLLECT variableName = expression INTO groupsVariable KEEP keepVariable options
COLLECT variableName = expression WITH COUNT INTO countVariable options
COLLECT variableName = expression AGGREGATE variableName = aggregateExpression options
COLLECT AGGREGATE variableName = aggregateExpression options
COLLECT WITH COUNT INTO countVariable options
```

选项在所有变体中都是可选的。


###  对语法进行分组

"COLLECT" 的第一个语法形式仅将结果按表达式中指定的组条件分组。为了进一步处理收集到的结果, 引入了一个新的变量 (由 variableName 指定)。此变量包含组值。

下面是一个查询, 它在美国城市中找到了不同的值, 并使它们可在可变城市中使用:

```
FOR u IN users
  COLLECT city = u.city
  RETURN {
    "city" : city
  }
```

第二个窗体与第一个窗体相同, 但另外引入了一个变量 (由 groupsVariable 指定), 其中包含掉到组中的所有元素。它的工作方式如下: groupsVariable 变量是一个数组, 其中包含的元素与组中的一样多。该数组的每个成员都是一个 JSON 对象, 其中在 AQL 查询中定义的每个变量的值都绑定到相应的属性。请注意, 这将考虑在收集语句之前定义的所有变量, 而不是在顶层 (任何一个 FOR) 的前面, 除非收集语句本身位于顶层, 在这种情况下, 所有变量都被采用。此外, 请注意, 优化程序可能会将语句移出以用于语句以提高性能。

```
FOR u IN users
COLLECT city = u.city INTO groups
RETURN {
  "city" : city,
  "usersInCity" : groups
}
```

在上面的示例中, 数组用户将按属性城市分组。结果是一个新的文档数组, 每个元素都有一个不同的 u. 城市值。元素从原始的数组 (这里: 用户) 每个城市被使可利用在可变的小组。这是由于进入条款。

"COLLECT" 还允许指定多个组条件。单个组条件可以用逗号分隔:

```
FOR u IN users
COLLECT country = u.country, city = u.city INTO groups
RETURN {
  "country" : country,
  "city" : city,
  "usersInCity" : groups
}
```

在上面的示例中, 数组用户首先按国家和城市分组, 对于每个不同的国家和城市组合, 用户将被返回。

###   丢弃过时的变量

第三种形式的COLLECT允许使用任意 projectionExpression 改写 groupsVariable 的内容:

```
FOR u IN users
COLLECT country = u.country, city = u.city INTO groups = u.name
RETURN {
  "country" : country,
  "city" : city,
  "userNames" : groups
}
```

在上面的例子中, 只有 projectionExpression 是 u 名称。因此, 仅将此属性复制到每个文档的 groupsVariable 中。这可能比将范围内的所有变量复制到 groupsVariable 中要有效得多, 因为它会在没有 projectionExpression 的情况下发生。

下面的表达式也可以用于任意计算:

```
FOR u IN users
COLLECT country = u.country, city = u.city INTO groups = {
  "name" : u.name,
  "isActive" : u.status == "active"
}
RETURN {
  "country" : country,
  "city" : city,
  "usersInCity" : groups
}
```

COLLECT还提供一个可选的保留子句, 可用于控制将哪些变量复制到创建的变量中。如果未指定保留子句, 则范围中的所有变量都将作为 sub-attributes 复制到 groupsVariable 中。这是安全的, 但如果范围内有许多变量或变量包含大量数据, 则会对性能产生负面影响。

下面的示例将复制到 groupsVariable 中的变量限制为仅名称。您和 someCalculation 在作用域中的变量也不会被复制到 groupsVariable 中, 因为它们没有在 "保留" 子句中列出:

```
FOR u IN users
LET name = u.name
LET someCalculation = u.value1 + u.value2
COLLECT city = u.city INTO groups KEEP name
RETURN {
  "city" : city,
  "userNames" : groups[*].name
}
```

保持是仅有效的与入的组合。在 "保留" 子句中只能使用有效的变量名。保持支持多个变量名的规范。

###   组长度计算

"COLLECT" 还提供了一个特殊的计数子句, 可用于有效地确定组成员的数量。

最简单的表单只返回使其进入collect的项的数量:

```
FOR u IN users
COLLECT WITH COUNT INTO length
RETURN length
```

上述内容等同于, 但效率高于:

```
RETURN LENGTH(
  FOR u IN users
    RETURN length
)
```

使用 count 子句还可以有效地计算每个组中的项数:

```
FOR u IN users
COLLECT age = u.age WITH COUNT INTO length
RETURN {
  "age" : age,
  "count" : length
}
```

#   聚合

COLLECT语句可用于执行每个组的数据聚合。为只确定组长度, 与计数入变异的收集可以使用如前面描述。

对于其他聚合, 可以对收集结果运行聚合函数:

```
FOR u IN users
COLLECT ageGroup = FLOOR(u.age / 5) * 5 INTO g
RETURN {
  "ageGroup" : ageGroup,
  "minAge" : MIN(g[*].u.age),
  "maxAge" : MAX(g[*].u.age)
}
```


---------------------------------------------


REMOVE
======

* remove * 关键字可用于从集合中移除文档。在一个
单台服务器, 则在事务中执行文档删除。
完全没有时尚。对于切分集合, 整个删除操作
不是事务性的。

每个 * remove * 操作仅限于单个集合, 而
[集合名称](../../手册/附录/词汇表. html # 集合名称) 不得为动态。
每个 AQL 查询只允许每个集合使用单 * 删除 * 语句, 并且
它不能后跟访问同一集合的读取操作,
遍历操作或可以读取文档的 AQL 函数。

删除操作的语法为:

```
REMOVE keyExpression IN collection options
```


* collection * 必须包含要删除文档的集合的名称
从。* keyExpression * 必须是包含文档标识的表达式。
这可以是一个字符串 (然后必须包含
[文档密钥](../../手册/附录/词汇表. html # 文档键) 或
文档, 它必须包含 * _key * 属性。

因此, 下列查询是等效的:

```
FOR u IN users
  REMOVE { _key: u._key } IN users

FOR u IN users
  REMOVE u._key IN users

FOR u IN users
  REMOVE u IN users
```

** 注意 **: 删除操作可以删除任意文档, 并且文档
不需要与前面的 * 声明所产生的相同:

```
FOR i IN 1..1000
  REMOVE { _key: CONCAT('test', i) } IN users

FOR u IN users
  FILTER u.active == false
  REMOVE { _key: u._key } IN backup
```

###  设置查询选项


* option * 可用于禁止在尝试
删除不存在的文档。例如, 以下查询将失败, 如果一个
to-be 删除的文档不存在:

```
FOR i IN 1..1000
  REMOVE { _key: CONCAT('test', i) } IN users
```

通过指定 * ignoreErrors * 查询选项, 可以抑制这些错误, 以便
查询完成:

```
FOR i IN 1..1000
  REMOVE { _key: CONCAT('test', i) } IN users OPTIONS { ignoreErrors: true }
```

为了确保在查询返回时已将数据写入磁盘, waitForSync *
查询选项:

```
FOR i IN 1..1000
  REMOVE { _key: CONCAT('test', i) } IN users OPTIONS { waitForSync: true }
```

### 返回已删除的文档


已删除的文档也可以由查询返回。在这种情况下, "REMOVE"
语句后面必须有一个 "RETURN" 语句 (中间的 ' LET ' 语句
也允许). "REMOVE" 引入了 pseudo-value "旧" 来引用已删除的
文件:

```
REMOVE keyExpression IN collection options RETURN OLD
```

下面是一个示例, 它使用名为 "已删除" 的变量来捕获被删除的
文件.对于每个已删除的文档, 将返回文档密钥。

```
FOR u IN users
  REMOVE u IN users
  LET removed = OLD
  RETURN removed._key
```

------------------------------------------------

UPDATE
======

* update * 关键字可用于部分更新集合中的文档。在一个
单一服务器, 更新执行事务在一个全有的时尚。
对于切分的集合, 整个更新操作不是事务性的。

每个 * UPDATE * 操作仅限于单个集合, 而
[集合名称](../../手册/附录/词汇表. html # 集合名称) 不得为动态。
每个 AQL 查询只允许每个集合的单 * UPDATE * 语句, 并且
它不能后跟访问同一集合的读取操作,
遍历操作或可以读取文档的 AQL 函数。
系统属性 * _id *, * _key * 和 * _rev * 不能更新, *_from * 和 *_to * 可以。

更新操作的两个语法是:

```
UPDATE document IN collection options
UPDATE keyExpression WITH document IN collection options
```

* collection * 必须包含文档的集合名称
进行更新。* document * 必须是包含属性和值的文档
要更新。使用第一个语法时, * document * 也必须包含 * _key *
属性来标识要更新的文档。

```js
FOR u IN users
  UPDATE { _key: u._key, name: CONCAT(u.firstName, " ", u.lastName) } IN users
```

下面的查询无效, 因为它不包含 * _key * 属性和
因此不可能确定要更新的文档:

```js
FOR u IN users
  UPDATE { name: CONCAT(u.firstName, " ", u.lastName) } IN users
```

使用第二个语法时, * keyExpression * 提供文档标识。
这可以是一个字符串 (随后必须包含文档密钥) 或
文档, 它必须包含 * _key * 属性。

下列查询是等效的:

```js
FOR u IN users
  UPDATE u._key WITH { name: CONCAT(u.firstName, " ", u.lastName) } IN users

FOR u IN users
  UPDATE { _key: u._key } WITH { name: CONCAT(u.firstName, " ", u.lastName) } IN users

FOR u IN users
  UPDATE u WITH { name: CONCAT(u.firstName, " ", u.lastName) } IN users
```

更新操作可能会更新不需要相同的任意文档
由前 * FOR * 的陈述所产生的部分:

```js
FOR i IN 1..1000
  UPDATE CONCAT('test', i) WITH { foobar: true } IN users

FOR u IN users
  FILTER u.active == false
  UPDATE u WITH { status: 'inactive' } IN backup
```

### 使用文档属性的当前值

"WITH" 子句中不支持 $this "OLD" (
  在 "更新" 之后可用)。若要访问当前属性值, 可以
  通常通过 "for" 循环的变量来引用文档, 这是用来
  循环访问集合:


```js
FOR doc IN users
  UPDATE doc WITH {
    fullName: CONCAT(doc.firstName, " ", doc.lastName)
  } IN users
```

如果没有循环, 因为单个文档只更新, 那么
可能不是像上面的变量 ("doc"), 这将让你引用
正在更新的文档:

```js
UPDATE "users/john" WITH { ... } IN users
```

若要在这种情况下访问当前值, 必须检索文档
并首先存储在变量中:

```js
LET doc = DOCUMENT("users/john")
UPDATE doc WITH {
  fullName: CONCAT(doc.firstName, " ", doc.lastName)
} IN users
```

可以通过这种方式修改现有属性的当前值,
要递增计数器, 例如:

```js
UPDATE doc WITH {
  karma: doc.karma + 1
} IN users
```

如果属性 "karma" 还不存在, "karma" 被评估为 * 为 null *。
该表达式 "null + 1" 导致新属性 "karma" 被设置为 * 1 *。
如果属性确实存在, 则它会增加 * 1 *。

当然, 数组也可以被突变:

```js
UPDATE doc WITH {
  hobbies: PUSH(doc.hobbies, "swimming")
} IN users
```

如果属性 "hobbies" 还不存在, 它就会被方便地初始化
作为 "[swimming]", 否则延长。

### 设置查询选项


* option * 可用于禁止在尝试
更新不存在的文档或违反唯一的键约束:

```js
FOR i IN 1..1000
  UPDATE {
    _key: CONCAT('test', i)
  } WITH {
    foobar: true
  } IN users OPTIONS { ignoreErrors: true }
```

更新操作将只更新 * document * 中指定的属性, 并
保持其他属性不变。内部属性 (如 * _id *, * _key *, * _rev *,
* _from * 和 * _to *) 不能更新, 并在 * document * 中指定时被忽略。
更新文档将使用服务器生成的值修改文档的修订号。

在更新具有 null 值的属性时, ArangoDB 不会删除该属性
从文档中, 但存储一个空值。删除更新中的属性
操作, 请将它们设置为 null 并提供 * keepNull * 选项:

```js
FOR u IN users
  UPDATE u WITH {
    foobar: true,
    notNeeded: null
  } IN users OPTIONS { keepNull: false }
```

上述查询将从文档中删除 * notNeeded * 属性, 并更新
* foobar * 属性正常。

还有一个选项 * mergeObjects *, 控制是否将对象内容
如果对象属性同时出现在 * UPDATE * 查询和
to-be 更新的文档。

以下查询将更新后的文档的 * name * 属性设置为精确
在查询中指定的值相同。这是由于 mergeObjects * 选项
被设置为 * false *:

```js
FOR u IN users
  UPDATE u WITH {
    name: { first: "foo", middle: "b.", last: "baz" }
  } IN users OPTIONS { mergeObjects: false }
```
相反, 下面的查询将合并 * name * 属性的内容。
具有查询中指定值的原始文档:

```js
FOR u IN users
  UPDATE u WITH {
    name: { first: "foo", middle: "b.", last: "baz" }
  } IN users OPTIONS { mergeObjects: true }
```

* name * 中存在于 to-be 更新的文档中的属性, 但不在
现在将保留查询。两者中存在的属性将被改写
在查询中指定的值。

注: * mergeObjects * 为 * true * 的默认值 , 因此无需指定
明确.

为了确保数据在更新查询返回时是持久的, 有 * waitForSync *
查询选项:


```js
FOR u IN users
  UPDATE u WITH {
    foobar: true
  } IN users OPTIONS { waitForSync: true }
```

### 返回修改后的文档


修改后的文档也可以由查询返回。在这种情况下, "UPDATE
语句需要遵循 "RETURN" 语句 (中间的 ' LET ' 语句
也是允许的)。这些语句可以引用 pseudo-values 的 "OLD" 和 "NEW"。
"OLD" pseudo-value 指更新前的文档修订, 以及 "NEW"
是指更新后的文档修订。

"OLD" 和 "NEW" 都将包含所有文档属性, 即使没有指定
在 update 表达式中。

```
UPDATE document IN collection options RETURN OLD
UPDATE document IN collection options RETURN NEW
UPDATE keyExpression WITH document IN collection options RETURN OLD
UPDATE keyExpression WITH document IN collection options RETURN NEW
```

下面是一个示例, 它使用名为 "previous" 的变量来捕获原始
修改前的文档。对于每个已修改的文档, 将返回文档密钥。
```js
FOR u IN users
  UPDATE u WITH { value: "test" }
  LET previous = OLD
  RETURN previous._key

```

下面的查询使用 "NEW" pseudo-value 返回更新的文档,
没有某些系统属性:


```js
FOR u IN users
  UPDATE u WITH { value: "test" }
  LET updated = NEW
  RETURN UNSET(updated, "_key", "_id", "_rev")
```

还可以返回 "旧" 和 "新":

```js
FOR u IN users
  UPDATE u WITH { value: "test" }
  RETURN { before: OLD, after: NEW }
```


----------------------------------------------------

REPLACE
=======

* REPLACE * 关键字可用于完全替换集合中的文档。在一个
单台服务器, 替换操作执行事务在一个全有-没有
时尚.对于切分集合, 整个替换操作不是事务性的。

每个 * REPLACE * 操作仅限于单个集合, 而
每个 AQL 查询只允许每个集合使用单 * REPLACE * 语句, 并且
它不能后跟访问同一集合的读取操作,
遍历操作或可以读取文档的 AQL 函数。
系统属性 * _id *, * _key * 和 * _rev * 不能被替换, * _from * 和 * _to * 可以。

替换操作的两个语法为:


```
REPLACE document IN collection options
REPLACE keyExpression WITH document IN collection options
```

* collection * 必须包含文档的集合名称
被替换。* document * 为替换文件。使用第一个语法时, * document *
还必须包含 * _key * 属性, 以标识要替换的文档。

```
FOR u IN users
  REPLACE { _key: u._key, name: CONCAT(u.firstName, u.lastName), status: u.status } IN users
```

下面的查询无效, 因为它不包含 * _key * 属性和
因此不可能确定要替换的文档:

```
FOR u IN users
  REPLACE { name: CONCAT(u.firstName, u.lastName, status: u.status) } IN users
```

使用第二个语法时, * keyExpression * 提供文档标识。
这可以是一个字符串 (随后必须包含文档密钥) 或
文档, 它必须包含 * _key * 属性。

下列查询是等效的:


```
FOR u IN users
  REPLACE { _key: u._key, name: CONCAT(u.firstName, u.lastName) } IN users

FOR u IN users
  REPLACE u._key WITH { name: CONCAT(u.firstName, u.lastName) } IN users

FOR u IN users
  REPLACE { _key: u._key } WITH { name: CONCAT(u.firstName, u.lastName) } IN users

FOR u IN users
  REPLACE u WITH { name: CONCAT(u.firstName, u.lastName) } IN users
```

替换将完全替换现有文档, 但不会修改值
内部属性 (如 * _id *, * _key *, * _from * 和 * _to *)。替换文档
将使用服务器生成的值修改文档的修订号。

替换操作可以更新不需要相同的任意文档
由前 * FOR * 的陈述所产生的部分:

```
FOR i IN 1..1000
  REPLACE CONCAT('test', i) WITH { foobar: true } IN users

FOR u IN users
  FILTER u.active == false
  REPLACE u WITH { status: 'inactive', name: u.name } IN backup
```

### 设置查询选项

* option * 可用于禁止在尝试
替换不存在的文档或在违反唯一键约束时:

```
FOR i IN 1..1000
  REPLACE { _key: CONCAT('test', i) } WITH { foobar: true } IN users OPTIONS { ignoreErrors: true }
```

为了确保在替换查询返回时数据是持久的, 有 * waitForSync *
查询选项:

```
FOR i IN 1..1000
  REPLACE { _key: CONCAT('test', i) } WITH { foobar: true } IN users OPTIONS { waitForSync: true }
```

### 返回修改后的文档


修改后的文档也可以由查询返回。在这种情况下, "REPLACE"
语句后面必须有一个 "RETURN" 语句 (中间的 ' LET' 语句是
允许的, 太)。"OLD" pseudo-value 可用于引用文档修订版之前
替换, "NEW" 是指替换后的文档修订。

"OLD" 和 "NEW" 都将包含所有文档属性, 即使没有指定
在 "替换" 表达式中。


```
REPLACE document IN collection options RETURN OLD
REPLACE document IN collection options RETURN NEW
REPLACE keyExpression WITH document IN collection options RETURN OLD
REPLACE keyExpression WITH document IN collection options RETURN NEW
```

下面是一个示例, 它使用名为 "previous" 的变量返回原始
修改前的文档。对于每个被替换的文档, 文档密钥将
返回:

```
FOR u IN users
  REPLACE u WITH { value: "test" }
  LET previous = OLD
  RETURN previous._key
```

下面的查询使用 "NEW" pseudo-value 返回替换的
文档 (不含某些系统属性):

```
FOR u IN users
  REPLACE u WITH { value: "test" }
  LET replaced = NEW
  RETURN UNSET(replaced, '_key', '_id', '_rev')
```

-----------------------------------------------------

INSERT
======

* INSERT * 关键字可用于将新文档插入到集合中。在一个
单服务器, 插入操作在事务中执行。
时尚.对于切分集合, 整个插入操作不是事务性的。

每个 * INSERT * 操作仅限于单个集合, 而
每个 AQL 查询只允许每个集合使用单 * INSERT * 语句, 并且
它不能后跟访问同一集合的读取操作,
遍历操作或可以读取文档的 AQL 函数。

插入操作的语法为:


```
INSERT document IN collection options
```

** 注 **: * INTO * 关键字也允许在 * IN *。

* collection * 必须包含文档的集合名称
入。* document * 是要插入的文档, 它可能包含也可能不包括
* _key * 属性。如果不提供 * _key * 属性, ArangoDB 将自动
值为 * _key * 值。插入文档也将自动文档
文档的修订号。

```js
FOR i IN 1..100
  INSERT { value: i } IN numbers
```

当插入到 **edge collection**,
在文档中指定属性 * _from * 和 * _to * 为必填项:

```js
FOR u IN users
  FOR p IN products
    FILTER u._key == p.recommendedBy
    INSERT { _from: u._id, _to: p._id } IN recommendations
```

### 设置查询选项


* option * 可用于禁止在违反唯一
关键约束:

```js
FOR i IN 1..1000
  INSERT {
    _key: CONCAT('test', i),
    name: "test",
    foobar: true
  } INTO users OPTIONS { ignoreErrors: true }
```

为了确保在插入查询返回时数据是持久的, 有 * waitForSync *
查询选项:

```js
FOR i IN 1..1000
  INSERT {
    _key: CONCAT('test', i),
    name: "test",
    foobar: true
  } INTO users OPTIONS { waitForSync: true }
```

### 返回插入的文档


插入的文档也可以由查询返回。在这种情况下, "INSERT"
语句可以是 "RETURN" 语句 (中间的 "LET" 语句也是允许的)。
要引用插入的文档, "INSERT" 语句引入了一个 pseudo-value
命名为 "NEW"。

"NEW" 中包含的文档将包含所有属性, 即使是自动生成的
数据库 (例如 "_id"、"_key"、"_rev")。


```js
INSERT document IN collection options RETURN NEW
```

下面是一个示例, 它使用名为 "inserted" 的变量返回插入的
文件.对于每个插入的文档, 将返回文档密钥:

```js
FOR i IN 1..100
  INSERT { value: i }
  LET inserted = NEW
  RETURN inserted._key
```
----------------------------------------------------


WITH
====

AQL 查询可以选择以 *WITH* 语句和
查询使用的集合。所有在 *WITH* 中指定的集合将
在查询开始时读锁定, 除了其他集合的查询
使用 AQL 查询分析器检测到的。

```
WITH managers, usersHaveManagers
FOR v, e, p IN OUTBOUND 'users/1' GRAPH 'userGraph'
  RETURN { v, e, p }
```




------------------------------------------------

*  译  [zhenruyan](https://github.com/zhenruyan)
