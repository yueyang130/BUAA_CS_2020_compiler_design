## 语法解读
1. 区分<整数>和<无符号整数>
2. 区分'['无符号整数']'和'['表达式']'
3. ＜语句列＞   ::= ｛＜语句＞｝, 这里的{}指的是0到无穷个，不是'{'


## 代码
1. 类似 ＜复合语句＞   ::=  ［＜常量说明＞］［＜变量说明＞］＜语句列＞ 的逻辑
<常量说明>的first与后面两个不交叉
<变量声明>的first与<语句列>不交叉
代码如下：
	// 两个if是并列的
	if (equal(symbolType::CONSTTK)) {
			ConstDeclare();
		}
	if (equal(symbolType::INTTK, symbolType::CHARTK)) {
		VarDeclare();
	}
	StatetmentList();