# Trabalho de Laboratório de Programação Paralela - BubbleSort

Realizado pelos alunos Caio Della Libera & Renan Cabrita Valladão


## Descrição do algoritmo

O Bubble Sort, ou ordenação por bolha, é um algoritmo de ordenação simples.

O algoritmo se dá percorrendo o vetor e trocando o elemento corrente com o próximo caso este seja menor que corrente.

O algoritmo repete esse processo, no pior caso, `n - 1` vezes, onde `n` é o número de elementos do vetor.

Ao final de cada iteração `i`, onde `i ∈ [0, n - 1]`, o elemento na posição `n - i` estará ordenado

A Figura 1 ilustra o algoritmo.

![Bubble Sort](https://lh5.googleusercontent.com/_oLwPF5ZvaZZ4pGD-HvSUSw6nTwwHjUwcLpNigUvb24-PKNwjMUwXcWYWf2wp4HopzHkh9JVmZd_AFYP4HjSYelidbw4FRo1fHrWV3KxbFM13xlRLALb-y-EbLhEmln11lhwEZPV)
(Figura 1)

O algoritmo, por precisar - no pior caso - de percorrer o array todo `n - 1` vezes, não é tão eficiente quanto algoritmos como o MergeSort. A complexidade computacional do BubbleSort é `O(n²)`

### Algoritmo Odd-Even

A paralelização do BubbleSort, a priori, não é possível de ser feita sem alterações no algoritmo. É necessário dividir bem os espaços de trabalho entre workers de modo a evitar condições de corrida.

Uma variação do algoritmo BubbleSort é o Odd-Even Sort, que divide cada iteração do algoritmo em dois passos:

- Passo Ímpar (Odd): Neste passo, apenas elementos com índice **ímpar** irão comparar seus valores com os próximos
- Passo Par (Even): Neste passo, apenas elementos com índice **par** irão comparar seus valores com os próximos

Dessa forma, haverá `n / 2` iterações executando os dois passos.

É possível, com a variação Odd-Even, fazer com que processos estejam responsáveis por regiões do vetor que não serão acessadas por outros.

A Figura 2 ilustra o processo

![Odd-Even Sort](https://media.geeksforgeeks.org/wp-content/uploads/20190505010351/oddEvenTranspositionSort.png)
(Figura 2)

## Algoritmo Paralelo

### Geeks For Geeks

### Artigo

## Aplicação Paralela

## Resultados

## Conclusões

## Referências

- https://www.geeksforgeeks.org/odd-even-transposition-sort-brick-sort-using-pthreads/

- http://www.hpcc.unn.ru/mskurs/ENG/DOC/pp10.pdf