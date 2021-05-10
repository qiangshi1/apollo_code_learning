# 2020年12月15日

文件：docker/scripts/dev_start.sh，docker/scripts/apollo_base.sh

2020年12月16日-大致读了一遍-把各个小命令拆分着看了一遍，还可以。

## 脚本里边初始化变量

test1.sh
``` 
A_VAR="avar"
echo "1"
export ${A_VAR}
echo "2"
export $A_VAR
echo "3"
echo $A_VAR
echo "4"
echo ${A_VAR}
# export 并不会输出内容，不知道为什么，只知道它和echo是不一样的！
```

会输出：
```
1
2
3
avar
4
avar
```

## 脚本加函数
``` 
func1 #同一命令行，第一次运行的话会报错，第二次可以正常运行，说明函数的声明是到命令行的环境变量里边去了
function func1(){
    echo "in func1"
}
func1
```

第一次会输出：
``` console
func1: command not found
in func1
```

第二次会输出：
``` console
in func1
in func1
```


## 函数加参数
``` 
function func2(){
    echo "in func2"
    echo "$@"
}
func2 "im param"
```

会输出：
``` console
in func2
im param
```

## 函数调用函数
```
function func1(){
    echo "in func1"
}
function func2(){
    func1
    echo "in func2"
}
func2
```

会输出：
``` console
in func1
in func2
```
 
## $()运算

括号里边是执行程序，整句话是把执行程序的返回生成变量，如：
```
echo $(cd / && ls)
```

会输出(\目录下的文件(夹)名称，根据自己的系统来定)：
``` console
bin boot cdrom core dev etc home initrd.img initrd.img.old lib lib64 lost+found media mnt opt proc root run sbin snap srv sys tmp usr var vmlinuz vmlinuz.old
```

## dirname
字符串操作，语义是输出当前文件的文件夹，如：
```
echo $(dirname "/1/2/3/")
```

会输出：
```console
/1/2
```

还有一个明命令叫basename：
```
echo $(basename "/1/2/3/")
```

会输出：
```console
3
```

## ${BASH_SOURCE[0]}
会输出当前的脚本名称

如：
```
echo ${BASH_SOURCE[0]} # in test2.sh
```

会输出：
```console
test2.sh
```

## x86_64
貌似本身就是一个阉割版的命令行

## uname

-a,-m,-s

## local

local一般用于局部变量声明，多在在函数内部使用。

- shell脚本中定义的变量是global的，其作用域从被定义的地方开始，到shell结束或被显示删除的地方为止。

- shell函数定义的变量默认是global的，其作用域从“函数被调用时执行变量定义的地方”开始，到shell结束或被显示删除处为止。函数定义的变量可以被显示定义成local的，其作用域局限于函数内。但请注意，函数的参数是local的。

- 如果同名，Shell函数定义的local变量会屏蔽脚本定义的global变量。

源自:https://blog.csdn.net/superbfly/article/details/49274889

如：


```
text="out of func3" 
echo $text
function func3()
{
    local text="in func3" #局部变量
    echo $text
}
func3
echo $text
```

会输出：
``` console
out of func3
in func3
out of func3
```

```
text="out of func3" 
echo $text
function func3()
{
    text="in func3" #局部变量
    echo $text
}
func3
echo $text
```

会输出：
``` console
out of func3
in func3
in func3
```


## if

文件表达式
- -e filename 如果 filename存在，则为真
- -d filename 如果 filename为目录，则为真 
- -f filename 如果 filename为常规文件，则为真
- -L filename 如果 filename为符号链接，则为真
- -r filename 如果 filename可读，则为真 
- -w filename 如果 filename可写，则为真 
- -x filename 如果 filename可执行，则为真
- -s filename 如果文件长度不为0，则为真
- -h filename 如果文件是软链接，则为真
- filename1 -nt filename2 如果 filename1比 filename2新，则为真。
- filename1 -ot filename2 如果 filename1比 filename2旧，则为真。


整数变量表达式
- -q 等于
- -e 不等于
- -t 大于
- -e 大于等于
- -t 小于
- -e 小于等于

字符串变量表达式
-   [ $a = $b ]                 如果string1等于string2，则为真 字符串允许使用赋值号做等号
-   [ $string1 !=  $string2 ]   如果string1不等于string2，则为真       
-   [ -n $string  ]             如果string 非空(非0），返回0(true)  
-   [ -z $string  ]             如果string 为空，则为真
-   [ $sting ]                  如果string 非空，返回0 (和-n类似) 

逻辑非 !                   条件表达式的相反
-  [ ! 表达式 ]
-  [ ! -d $num ]               如果不存在目录$num


辑与 –a                   条件表达式的并列
-  [ 表达式1  –a  表达式2 ]


逻辑或 -o                   条件表达式的或
-  [ 表达式1  –o 表达式2 ]

源自:https://www.cnblogs.com/smallredness/p/11054882.html

如：
```
if [ -e "test2.sh" ]; then #存在test2.sh文件 另外，可以看出if后跟的条件语句是用中括号括起来的
    echo "test2.sh exists!"
fi
```

会输出：

```
test2.sh exists!
```

## function可以用return提前返回

## exit

表示退出命令行，

可后加code，如：


```
exit 1
```

vscode 会报错（似乎命令行不会）：
```
The terminal process "/bin/bash" terminated with exit code: 1.
```

## cat

文件内容写入，具体参见:https://www.runoob.com/linux/linux-comm-cat.html


## tee

还有这奇葩命令？？

具体参见:https://www.runoob.com/linux/linux-comm-tee.html。如：
```
echo "biubiubiu" | tee -a test.txt
```

屏幕输出biubiubiu同时把biubiubiu增量写入到test.txt

## read

屏幕输入，如：
```
read RP # 命令行输入 biubiubiu
echo $RP # 命令行会输出biubiu比u
```

参考: https://www.runoob.com/linux/linux-comm-read.html

大误：为什么会有个-r？

## echo  1 >> 2.txt

把1作为内容输入到2.txt中去，另一个类似的命令是：
```
cat 1.txt >> 2.txt  #把1.txt的内容输入到2.txt中去。
```

## =~

正则匹配（相对的是==），如：
```
AVAR=1
[[ $AVAR =~ ^[0-9]+ ]] && echo "is a number." || echo "not a numer!"
AVAR=b
[[ $AVAR =~ ^[0-9]+ ]] && echo "is a number." || echo "not a numer!"
```

会输出：
``` console
is a number.
not a numer!
```

## EOF

换行用的，参考:https://blog.csdn.net/zongshi1992/article/details/71693045

## for

摘自:https://www.cnblogs.com/EasonJim/p/8315939.html

##### 1
```
for((i=1;i<=4;i++));  
do   
echo $(expr $i \* 3 + 1);  # i*3+1
done 
```

会输出：
``` console
4
7
10
13
```
##### 2
```
for i in $(seq 1 4)  
do   
echo $(expr $i \* 3 + 1);  # i*3+1
done  
```

会输出：
``` console
4
7
10
13
```
##### 3
```
for i in {1..4}  
do  
echo $(expr $i \* 3 + 1);  # i*3+1
done  
```

会输出：
``` console
4
7
10
13
```
##### 4
```
for i in `ls`; #执行ls命令
do   
echo $i is file name\! ; #输出当前文件夹下可以ls出的文件
done 
```
##### 5
```
for i in $* ;  #放到脚本里执行，后接参数，如biubiubiu，
do  
echo $i is input chart\! ;  #会输出biubiubiu is input chart!
done
```

会输出：
``` console
4
7
10
13
```
##### 6
```
for i in f1 f2 f3 ;  
do  
echo $i is appoint ;  
done 
```

会输出：
``` console
f1 is appoint
f2 is appoint
f3 is appoint
```

##### 7
```
list="rootfs usr data data2"  #空格可以作为分隔符
for i in $list;  
do  
echo $i is appoint ;  
done  
```

会输出：
``` console
rootfs is appoint
usr is appoint
data is appoint
data2 is appoint
```

## while

##### 1
```
i=1
sum=0
while [ $i -le 10 ] # -le小于等于
do
  let sum=sum+i #等价于let sum=sum+$i； 等价于let sum=$sum+$i； 不等价于let $sum=$sum+$i;
  #另外若果删除let sum=$sum+$i则会变成字符串操作，你可以试一下
  let i++
done
echo $sum
```

会输出：
``` console
55
```

##### 2
```
i=1
j=1
while [  $i  -le  5  ]
do
  while [  $j  -le  5  ]
  do
     echo -n  "*  "
     let j++
  done
  echo
  let  i++
  let  j=1
done
```

会输出：
``` console
*  *  *  *  *  
*  *  *  *  *  
*  *  *  *  *  
*  *  *  *  *  
*  *  *  *  * 
```
摘自：https://blog.csdn.net/wdz306ling/article/details/79602739

和while类似的一个关键词是until，它正好是和while反着来的

## -eq -ne -gt -lt -ge -le 

- -eq           //等于
- -ne           //不等于
- -gt            //大于 （greater ）
- -lt            //小于  （less）
- -ge            //大于等于
- -le            //小于等于
 
命令的逻辑关系：
- 在linux 中 命令执行状态：0 为真，其他为假
- 逻辑与： &&
- 第一个条件为假时，第二条件不用再判断，最终结果已经有；
- 第一个条件为真时，第二条件必须得判断；
- 逻辑或： ||
- 逻辑非： ！

摘自：https://www.cnblogs.com/jxhd1/p/6274854.html

## shift

```
# 脚本输入，后接参数： 1 2 3 4
while [ $# -ne 0 ]  # $#参数个数 $number 第number个参数
do
echo "第一个参数为: $1 参数个数为: $#"
shift   #参数列表左移
done
```

会输出：

``` console
第一个参数为: 1 参数个数为: 4
第一个参数为: 2 参数个数为: 3
第一个参数为: 3 参数个数为: 2
第一个参数为: 4 参数个数为: 1
```

## case

```
#判断用户输入

#在屏幕上输出"请选择yes/no"，然后把用户选择赋予变量cho
read -p "Please choose yes/no: " -t 30 cho
case $cho in
#判断变量cho的值
    "yes")
    #如果是yes
        echo "Your choose is yes!"
        #则执行程序1
        ;;
    "no")
    #如果是no
        echo "Your choose is no!"
        #则执行程序2
        ;;
    *)
    #如果既不是yes,也不是no
    echo "Your choose is error!"
    #则执行此程序
    ;;
esac
```

摘自：http://c.biancheng.net/view/1003.html

## eval

解析并执行字符串中的命令

```
name=woodie
cmd="echo Helllo $name\! "
eval $cmd # 会输出：Hello woodie!
```

参考： https://www.cnblogs.com/triple-y/p/11236082.html

## nohub

nohup 英文全称 no hang up（不挂起），用于在系统后台不挂断地运行命令，退出终端不会影响程序的运行。

nohup 命令，在默认情况下（非重定向时），会输出一个名叫 nohup.out 的文件到当前目录下，如果当前目录的 nohup.out 文件不可写，输出重定向到 $HOME/nohup.out 文件中。

参考： https://www.runoob.com/linux/linux-comm-nohup.html


