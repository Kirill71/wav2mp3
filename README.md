# wav2mp3
<h1>Simple convertor wav to mp3</h1>

<h2>Conditions</h2>
<ol>
<li>The application is called with pathname as argument, all WAV-files contained directly in that folder are to be encoded to MP3;</li>
<li>Use all available CPU cores for the encoding process in an efficient way by utilizing multi-threading;</li>
<li>Statically link to lame encoder library;</li>
<li>Application should be compilable and runnable on Windows and Linux;</li>
<li>The resulting MP3 files are to be placed within the same directory as the source WAV files, the filename extension should be changed appropriately to .MP3
non-WAV files in the given folder shall be ignored;</li>
<li>Multithreading shall be implemented by means of using Posix Threads;</li>
<li>The Boost library shall not be used;</li>
<li>The LAME encoder should be used with reasonable standard settings.</li>
</ol>  

<h2>Linux</h2>
Buildable on any more-or-less modern distribution (gcc 4.8+) as easy as "cmake . && make".
Static libmp3lame library must be installed to /usr/local/lib prior to building app or you can use local lame lib 
just configure CMake with flag "-DUSE_LOCAL_LAME_LIB=TRUE".

<h2>Visaul Studio</h2>
Latest cmake properly work with latest Visual Studio since 19 version.
Project can be opened in VS and will be built.
You don't need any additional steps.

  

