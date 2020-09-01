## About Work
This work was originally published in 4 August 2020 at APAN50 by 
- Mr Chanpol Kongsute
- Assoc. Prof. Dr Chaodit Aswakul

## The objective
we propose a pragmatic implementation framework in which Kubernetes clusters are federated in application-level as a peer-to-peer federation through Istio service mesh. This framework can be instantiated by configuring an Istio gateway to act as a cross-cluster load balancer handling traffic between clusters. This approach allows use cases such as cloud bursting and canary deployment. As a verification of this proposed federation framework, we use two Kubernetes clusters installed with echo servers. And by sending requests to the echo server locating in the cluster, we observe the behaviors and limitations of our proposed model.

