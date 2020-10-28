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

## 错误
1. vector的元素类型不能是引用，因为引用必须在定义时初始化。
2. 不能使用不完整的类型的意思是：没有include对应的头文件。
3. 类的非静态方法一般不能作为函数指针，因为它有一个隐藏参数this，还要解决继承等问题。要使用类成员函数指针。
		(void(GrammerAnalyzer::*handler)(symbolType) = &GrammerAnalyzer::ConstValue;  // 取函数指针 
		(obj/this->*handler)(var_type); // 使用函数指针
4. 与Java不同，使用类的静态方法时，不能使用.运算符，要使用::运算符。
5.在C语言中，定义结构体:
	typedef strcut {
		...
	} A;
而在C++中，不需要使用typedef,而且将结构体名放在前面。
	struct A {
		...
	};

## 调试技巧
1. 在调试状态下，调试->windows->调用堆栈，可以查看函数调用层次
