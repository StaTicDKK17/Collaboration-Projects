#!/usr/bin/env bash

# Exit immediately if any command below fails.
set -e

make


echo "Generating a test_files directory.."
mkdir -p test_files
rm -f test_files/*


echo "Generating test files.."

# empty
printf "" > test_files/empty.input

# ascii
printf "Hello, World!\n" > test_files/ascii_1.input
printf "Hello, World!" > test_files/ascii_2.input
printf "             " > test_files/ascii_3.input
printf "\x07\x07\x07" > test_files/ascii_4.input
printf "##" > test_files/ascii_5.input
printf "\x07\n\x07\n\x07\n" > test_files/ascii_6.input
printf "\n\n" > test_files/ascii_7.input
printf "#\n" > test_files/ascii_8.input
printf "printf(\"@#@\")" > test_files/ascii_9.input
printf "~\n~" > test_files/ascii_10.input
printf "  " > test_files/ascii_11.input
printf "\x0d\x0d" > test_files/ascii_12.input


# ISO-8859
printf "\x70\x72\xf8\x76\x65\x72\x20\x65\x72\x20\x64\xe5\x72\x6c\x69\x67\x65" > test_files/iso8859_1.input
printf "\xE5\xF8\xE6" > test_files/iso8859_2.input
printf "\x61\x62\x63\x64\x65\x66\x67\x68\x69\x6a\x6b\x6c\x6d\x6e\x6f\x70\x71\x72\x73\x74\x75\x76\x77\x78\x79\x7a\xe6\xf8\xe5" > test_files/iso8859_3.input
printf "\x6d\xf2\x72" > test_files/iso8859_4.input
printf "\x6e\xeb\x6e\xeb\x20\x6b\x75\x71\n" > test_files/iso8859_5.input
printf "\x3f\x3f\x3f\x3f\x3f\x3f\x3f\n" > test_files/iso8859_6.input
printf "\xc9\x67\x20\x66\xf3\x72\x20\x74\x69\x6c\x20\x42\x72\x65\x74\x6c\x61\x6e\x64\x73\x20\xfe\x65\x67\x61\x72\x20\xe9\x67\x20\x76\x61\x72\x20\x65\x69\x6e\x73\x20\xe1\x72\x73" > test_files/iso8859_7.input
printf "\xa1\xa2\xae\xd1\xde\xfb" > test_files/iso8859_8.input
printf "\xFFxFF" > test_files/iso8859_9.input
printf "\x7E \x7E" > test_files/iso8859_10.input
printf "\xA0 \xA0" > test_files/iso8859_11.input
printf "\A1" > test_files/iso8859_12.input



# UTF-8
printf "ˤ" > test_files/utf8_1.input
printf "ˤˤˤ" > test_files/utf8_2.input
printf "𒀱ˤ" > test_files/utf8_3.input
printf "¡" > test_files/utf8_4.input
printf "¼¼" > test_files/utf8_5.input
printf "⌨" > test_files/utf8_6.input
printf "⎲⎳" > test_files/utf8_7.input
printf "𒁃" > test_files/utf8_8.input
printf "𒁆" > test_files/utf8_9.input
printf "𒀱" > test_files/utf8_10.input
printf "𒁃𒀱" > test_files/utf8_11.input
printf "ˤ ˤˤˤ \a ¡ ¼¼ ⌨ ⎲\n⎳ 𒁃 𒁆 𒀱 𒁃𒀱" > test_files/utf8_12.input


#data
printf "Hello,\x00World!\n" > test_files/data_1.input
printf "Hello \xff\x11" > test_files/data_2.input
printf "Hello \xff\x7f" > test_files/data_3.input
printf "Hello \xfe\x0e" > test_files/data_4.input
printf "\x78\xBF\x04\xB0" > test_files/data_5.input
printf "\x78\n\xBF\n\x04\n\xB0\n" > test_files/data_6.input
printf "\n\n\n\n\n\n\n\xF0\n\n" > test_files/data_7.input
printf "\x3f\xff\x00\x3f\x3f\x3f\x3f\n" > test_files/data_8.input
printf "\x00\x00\xF0\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00" > test_files/data_9.input
printf "\x00ˤ ˤˤˤ \a ¡ ¼¼ ⌨ ⎲\n⎳ 𒁃 𒁆 𒀱 𒁃𒀱" > test_files/data_10.input
printf "ˤ ˤˤˤ \a ¡ ¼¼ ⌨ ⎲\n⎳ 𒁃 𒁆 𒀱 𒁃𒀱\x00" > test_files/data_11.input
printf "\x02\xF1\x00" > test_files/data_12.input





echo "Running the tests.."
exitcode=0
for f in test_files/*.input
do
  echo ">>> Testing ${f}.."
  file    ${f} | sed -e 's/ASCII text.*/ASCII text/' \
                         -e 's/Unicode text, UTF-8.*/UTF-8 text/' \
                         -e 's/ISO-8859 text.*/ISO-8859 text/' \
                         -e 's/writable, regular file, no read permission/cannot determine (Permission denied)/' \
                         > "${f}.expected"
  ./file  "${f}" > "${f}.actual"

  if ! diff -u "${f}.expected" "${f}.actual"
  then
    echo ">>> Failed :-("
    exitcode=1
  else
    echo ">>> Success :-)"
  fi
done
exit $exitcode
