#!/bin/bash
linea="A, NO, SI, B"

echo "$linea" | awk -F "," '{print $1}'



ps aux | awk '{print $1}'