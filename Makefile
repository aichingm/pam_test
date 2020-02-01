
USER = Ragnar
PASSWORD = Lodbrok
EXPAND=

.PHONY: build clean nice install uninstall root test

build: clean auth module.so

clean:
	rm -rf *.so *.o auth *.233r.expand graph.pdf

nice:
	clang-format -i -style=Chromium *.c

auth: auth.c
	gcc -g $(EXPAND) -lpam -o auth auth.c

module.so: module.c
	gcc -c -g $(EXPAND) module.c -o module.o
	ld -x --shared -o module.so module.o

install: root
	cp module.so /usr/lib/security/pam_test.so
	echo "#%PAM-1.0" > /etc/pam.d/pam_test
	echo "auth required pam_test.so $(USER) $(PASSWORD)" >> /etc/pam.d/pam_test

test:
	./auth pam_test $(USER) $(PASSWORD)

uninstall: root
	rm /etc/pam.d/pam_test /usr/lib/security/pam_test.so

root:
	@[[ $$(id -u) -eq "0" ]] || (echo -e "\e[01;31mYou Need To Be Root!\e[0m" && exit 1)

graph.pdf: auth.c module.c
	make build EXPAND=-fdump-rtl-expand
	egypt *.233r.expand | dot -Tpdf -o graph.pdf
	rm *.233r.expand
