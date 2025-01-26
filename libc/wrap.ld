// 备份原始的函数符号
PROVIDE(__real_function_to_test = function_to_test);
// 将原始函数符号重定向到包装函数
function_to_test = __wrap_function_to_test;