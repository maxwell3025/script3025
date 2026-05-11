Expression
```
replace
  (add[0x000x555555ce2260] 0 a[0x000x555555ce2210])
  (λ(index: Nat).
   λ(value: add[0x000x555555ce2260] 0 a[0x000x555555ce2210]=index).
   Nat)
```

Type
```
Π(minor_premise: (λ(index: Nat).
                  λ(value: add[0x000x555555ce2260] 0 a[0x000x555555ce2210]=index).
                  Nat) inductive (refl inductive)).
Π(major_premise_index: Nat).
Π(major_premise: inductive=major_premise_index).
(λ(index: Nat). λ(value: add[0x000x555555ce2260] 0 a[0x000x555555ce2210]=index). Nat) major_premise_index major_premise
```