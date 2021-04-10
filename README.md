# machdump

![machdump in action](/img/example.png)

`machdump` is a tool to verbatim dump Mach-O object files for low-level debugging. I wrote it to aid in development for my language _chrona_.

> NOTE: `machdump` only compiles on MacOS where the Mach-O headers are defined.

Simply give it one or more mach-on files on the command and it will dump each. It also responds to the universal options `--help` and `--version`.

## Usage

I could not be bothered to write a makefile since compiling this is as simple as:
```bash
git clone https://github.com/euppal/machdump
cd machdump
$CC -Iinclude src/*.c -O2 -o machdump machdump
```
You can then install machdump with
```bash
sudo cp machdump /usr/local/bin/machdump
```
