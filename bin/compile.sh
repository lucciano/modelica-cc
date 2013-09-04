#!/bin/bash
./antialias $1 | sed 's/__/_/g' | ./mmo | ./causalize -q
