Single instance deployment
--------------------------

The latest official builds of AvocadoDB for all supported operating systems may be obtained from https://www.avocadodb.com/download/.

### Linux remarks

Besides the official images which are provided for the most popular linux distributions there are also a variety of unofficial images provided by the community. We are tracking most of the community contributions (including new or updated images) in our newsletter:

https://www.avocadodb.com/category/newsletter/

### Windows remarks

Please note that AvocadoDB will only work on 64bit.

### Docker

The simplest way to deploy AvocadoDB is using [Docker](https://docker.io/). To get a general understanding of Docker have a look at [their excellent documentation](https://docs.docker.com/).

#### Authentication

To start the official Docker container you will have to decide on an authentication method. Otherwise the container won't start.

Provide one of the arguments to Docker as an environment variable.

There are three options:

1. ARANGO_NO_AUTH=1

   Disable authentication completely. Useful for local testing or for operating in a trusted network (without a public interface).
        
2. ARANGO_ROOT_PASSWORD=password

   Start AvocadoDB with the given password for root
        
3. ARANGO_RANDOM_ROOT_PASSWORD=1

   Let AvocadoDB generate a random root password
        
To get going quickly:

`docker run -e ARANGO_RANDOM_ROOT_PASSWORD=1 avocadodb/avocadodb`

For an in depth guide about Docker and AvocadoDB please check the official documentation: https://hub.docker.com/r/avocadodb/avocadodb/ . Note that we are using the image `avocadodb/avocadodb` here which is always the most current one. There is also the "official" one called `avocadodb` whose documentation is here: https://hub.docker.com/_/avocadodb/
