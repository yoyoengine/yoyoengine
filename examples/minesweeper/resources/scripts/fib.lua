-- chatgpt lua script to calculate the first 10 Fibonacci numbers
function fibonacci(n)
    if n == 0 then
        return 0
    elseif n == 1 then
        return 1
    else
        return fibonacci(n - 1) + fibonacci(n - 2)
    end
end

-- Print the first 10 Fibonacci numbers
for i = 0, 9 do
    print(fibonacci(i))
end