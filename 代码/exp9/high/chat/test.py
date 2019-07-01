import chat as cb
import sys
while True:
    print("YOU>", end="")
    sentence = sys.stdin.readline()
    print("AI>", cb.chat(sentence))
