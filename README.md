# js-cmd
JavaScript on command line with ChakraCore for macOS

# usage

```
$ js 1+1
$ js "2*3"
$ js 0xff
$ js "new Date().getTime()"
$ js "p(2); console.log(1);"
$ js `
for (let i = 0; i < 10; i++) {
  p(i);
}
`
```

# API

## console.log()

output to console (like printf)

## p()

same as consoel.log()

# install

```
$ sudo cp js /usr/local/bin/
```


# build

build ChakraCore  
https://github.com/microsoft/ChakraCore  

set LIBRARY_PATH on Makefile  
```
$ make
```
