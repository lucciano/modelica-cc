#!/bin/bash
./antialias $1 | ./mmo | ./causalize -q | sed 's/__/_/g'
