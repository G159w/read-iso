#!/bin/sh
red=$'\e[31m'
green=$'\e[32m'
white=$'\e[39m'
exe="$1"
path="$2"

compare()
{
  echo "Test with : my_read_iso < tests/commands/$1"
  me=$(./my_read_iso $path < tests/commands/$1)
  me_det=$(echo $?)
  sys=$(./$exe $path < tests/commands/$1)
  sys_det=$(echo $?)
  if [ "$me" == "$sys" ]; then
    printf "output : %s[ OK ]%s " ${green}  ${white}
  else
    printf "output : %s[ KO ]%s " ${red} ${white}
  fi
  
  if [ "$me_det" == "$sys_det" ]; then
    printf "return : %s[ OK ]%s\n" ${green} ${white}
  else
    printf "return : %s[ KO ]%s\n" ${red} ${white}
  fi
}
printf "%s========================================" ${green}
printf "GOOD COMMANDS"
printf "==========================================%s\n"  ${white}
compare "help"
compare "info"
compare "ls"
compare "catread"
compare "cdmp3"
compare "tree"
printf "%s=======================================" ${red}
printf "WRONG COMMANDS"
printf "==========================================%s\n"  ${white}
compare "wrongcomm"
compare "wrongspace"
compare "wrongarg"
compare "wrongcd1"
compare "wrongcd2"
compare "wrongcatarg1"
compare "wrongcatarg2"
printf "%s========================================" ${green}
printf "BIG %sCOMMANDS" ${red}
printf "==========================================%s\n"  ${white}
compare "bigcomm"

