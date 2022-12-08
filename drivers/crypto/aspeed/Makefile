<<<<<<< HEAD
hace-hash-$(CONFIG_CRYPTO_DEV_ASPEED_HACE_HASH) := aspeed-hace-hash.o
hace-crypto-$(CONFIG_CRYPTO_DEV_ASPEED_HACE_CRYPTO) := aspeed-hace-crypto.o

obj-$(CONFIG_CRYPTO_DEV_ASPEED) += aspeed_crypto.o
aspeed_crypto-objs := aspeed-hace.o	\
		      $(hace-hash-y)	\
		      $(hace-crypto-y)

aspeed_acry-$(CONFIG_CRYPTO_DEV_ASPEED_ACRY) += aspeed-acry.o

obj-$(CONFIG_CRYPTO_DEV_ASPEED) += $(aspeed_acry-y)
=======
hace-hash-$(CONFIG_CRYPTO_DEV_ASPEED_HACE_HASH) := aspeed-hace.o aspeed-hace-hash.o
hace-crypto-$(CONFIG_CRYPTO_DEV_ASPEED_HACE_CRYPTO) := aspeed-hace.o aspeed-hace-crypto.o

obj-$(CONFIG_CRYPTO_DEV_ASPEED) += aspeed_crypto.o
aspeed_crypto-objs := $(hace-hash-y)	\
		      $(hace-crypto-y)
>>>>>>> b7ba80a49124 (Commit)
