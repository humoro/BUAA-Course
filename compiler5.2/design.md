#### 大概步骤

1. 解决静态存储的分配：全局变量的内存分配（`mips`中全局内存分配）

2. 动态内存的分配，对于每个程序块（复合语句为划分），分配栈空间，并且记录每个变量的栈空间位置

3. 设计四元式：

   关于各个终结符语法现象的处理办法(生成中间代码)：

   * 字符串：字符串只会出现在输出的部分，在处理到字符串的时候
     
     * 打印字符串，中间代码模式`print.str str`
   * 变量声明：`const.var varname varvalue` `plant.var varname varsize`
   * 对于表达式的四元式：
     
     * 加减乘除` op num1 num2 ans`（可以存在`+-`缺失一个操作数的情况`ans = num1 op num2`）
   * 各种语句类型的处理：
     * 中间代码模式仍然是`if-else`模式，但是其中代码块使用`goto`进行跳转
     
       ```
       if (简化后的条件) goto label_if
       else goto label_else
       label_if
       statement_list
       goto label_code
       label_else
       statement_list
       goto label_code
       ...
       label_code
       无论在if还是else中对应标签的代码块中最后一句为 goto正常代码执行区
       ```
     
     * 循环语句
       ```
       for (i = 0;i < 0;i = i + 1) {
       	statement_list;
       }
       while (i < 0) {
       	statement_list
       }
       
       i = 0
       label_cycle
       if (i >= 0) goto label_code
       statement_list
       i = i + 1
       goto label_cycle
       label_code
       
       对于for循环把步长语句添加到代码块最后中
       while 循环语句中不需要初始化变量
       
       do{
       statement_list
       } while (i > 0)
       
       label_do.num://其中的num是为了区别各个标签
       	statement_list
       if (i > 0) goto label_do
       ```
       
     * 赋值语句：`label = num/label' ` `数组赋值语句a = x[i] x[i] = ...`
     
     *  读语句：`read.int symbol` `read.char symbol`
     
     * 打印语句：`print.sym symbol` `print_str str`
     
     * 返回语句`return symbol`，对于返回语句中表达式，要先进行处理得到最终结果的`symbol`；无返回值函数如果没有返回语句需要添加
     
     * 函数调用语句：
     
       * 无返回值函数调用：
     
         ```
         funcname_para.num symbol
         .
         .
         .
         voidfunc_call funcname
         ```
     
       * 返回值函数调用：
     
         ```
         funcname_para.num symbol
         .
         .
         .
         retfunc_call funcname labelstore//存储返回值
         ```
     
     * 函数声明：
     
       * 声明语句`function.funcname:` 
       * 参数表：`function.def.para`
   
4. 对于中间代码的一些约定：

   1. 中间添加的变量同一使用`var.num` `num`是维护的全局变量每次声明使用一个变量`num++`
   2. 每个函数定义中函数结束之前都要有返回语句（无论是否可达）

5. 中间代码放入一个统一的`stringbuf对象中`





​                