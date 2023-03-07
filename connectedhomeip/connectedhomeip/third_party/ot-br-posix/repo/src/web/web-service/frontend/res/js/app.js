/*
 *    Copyright (c) 2017, The OpenThread Authors.
 *    All rights reserved.
 *
 *    Redistribution and use in source and binary forms, with or without
 *    modification, are permitted provided that the following conditions are met:
 *    1. Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *    2. Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *    3. Neither the name of the copyright holder nor the
 *       names of its contributors may be used to endorse or promote products
 *       derived from this software without specific prior written permission.
 *
 *    THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 *    AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 *    IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 *    ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 *    LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 *    CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 *    SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 *    INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 *    CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 *    ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 *    POSSIBILITY OF SUCH DAMAGE.
 */

(function() {
    angular
        .module('StarterApp', ['ngMaterial', 'ngMessages'])
        .controller('AppCtrl', AppCtrl)
        .service('sharedProperties', function() {
            var index = 0;
            var networkInfo;

            return {
                getIndex: function() {
                    return index;
                },
                setIndex: function(value) {
                    index = value;
                },
                getNetworkInfo: function() {
                    return networkInfo;
                },
                setNetworkInfo: function(value) {
                    networkInfo = value
                },
            };
        });

    function AppCtrl($scope, $http, $mdDialog, $interval, sharedProperties) {
        $scope.menu = [{
                title: 'Home',
                icon: 'home',
                show: true,
            },
            {
                title: 'Join',
                icon: 'add_circle_outline',
                show: false,
            },
            {
                title: 'Form',
                icon: 'open_in_new',
                show: false,
            },
            {
                title: 'Status',
                icon: 'info_outline',
                show: false,
            },
            {
                title: 'Settings',
                icon: 'settings',
                show: false,
            },
            {
                title: 'Commission',
                icon: 'add_circle_outline',
                show: false,
            },
            {
                title: 'Topology',
                icon: 'add_circle_outline',
                show: false,
            },

        ];

        $scope.thread = {
            networkName: 'OpenThreadDemo',
            extPanId: '1111111122222222',
            panId: '0x1234',
            passphrase: 'j01Nme',
            networkKey: '00112233445566778899aabbccddeeff',
            channel: 15,
            prefix: 'fd11:22::',
            defaultRoute: true,
        };

        $scope.setting = {
            prefix: 'fd11:22::',
            defaultRoute: true,
        };

        $scope.headerTitle = 'Home';
        $scope.status = [];

        $scope.isLoading = false;

        $scope.showScanAlert = function(ev) {
            $mdDialog.show(
                $mdDialog.alert()
                .parent(angular.element(document.querySelector('#popupContainer')))
                .clickOutsideToClose(true)
                .title('Information')
                .textContent('There is no available Thread network currently, please \
                             wait a moment and retry it.')
                .ariaLabel('Alert Dialog Demo')
                .ok('Okay')
            );
        };
        $scope.showPanels = function(index) {
            $scope.headerTitle = $scope.menu[index].title;
            for (var i = 0; i < 7; i++) {
                $scope.menu[i].show = false;
            }
            $scope.menu[index].show = true;
            if (index == 1) {
                $scope.isLoading = true;
                $http.get('available_network').then(function(response) {
                    $scope.isLoading = false;
                    if (response.data.error == 0) {
                        $scope.networksInfo = response.data.result;
                    } else {
                        $scope.showScanAlert(event);
                    }
                });
            }
            if (index == 3) {
                $http.get('get_properties').then(function(response) {
                    console.log(response);
                    if (response.data.error == 0) {
                        var statusJson = response.data.result;
                        $scope.status = [];
                        for (var i = 0; i < Object.keys(statusJson).length; i++) {
                            $scope.status.push({
                                name: Object.keys(statusJson)[i],
                                value: statusJson[Object.keys(statusJson)[i]],
                                icon: 'res/img/icon-info.png',
                            });
                        }
                    }
                });
            }
            if (index == 6) {
                $scope.dataInit();
                $scope.showTopology();
            }
        };

        $scope.showJoinDialog = function(ev, index, item) {
            sharedProperties.setIndex(index);
            sharedProperties.setNetworkInfo(item);
            $scope.index = index;
            $mdDialog.show({
                controller: DialogController,
                templateUrl: 'join.dialog.html',
                parent: angular.element(document.body),
                targetEvent: ev,
                clickOutsideToClose: true,
                fullscreen: $scope.customFullscreen,
            });
        };

        function DialogController($scope, $mdDialog, $http, $interval, sharedProperties) {
            var index = sharedProperties.getIndex();
            $scope.isDisplay = false;
            $scope.thread = {
                networkKey: '00112233445566778899aabbccddeeff',
                prefix: 'fd11:22::',
                defaultRoute: true,
            };

            $scope.showAlert = function(ev, message) {
                $mdDialog.show(
                    $mdDialog.alert()
                    .parent(angular.element(document.querySelector('#popupContainer')))
                    .clickOutsideToClose(true)
                    .title('Information')
                    .textContent(message)
                    .ariaLabel('Alert Dialog Demo')
                    .ok('Okay')
                    .targetEvent(ev)
                );
            };

            $scope.showQRAlert = function(ev, message) {
                $mdDialog.show(
                    $mdDialog.alert()
                    .parent(angular.element(document.querySelector('#popupContainer')))
                    .clickOutsideToClose(true)
                    .title('Information')
                    .textContent(message)
                    .ariaLabel('Alert Dialog Demo')
                    .ok('Okay')
                    .targetEvent(ev)
                    .multiple(true)
                );
            };

            $scope.showQRCode = function(ev, image) {
              $mdDialog.show({
                targetEvent: ev,
                parent: angular.element(document.querySelector('#popupContainer')),
                  template:
                    '<md-dialog>' +
                    '  <md-dialog-content>' +
                    '  <h6 style="margin: 10px 10px 10px 10px; "><b>Open your OT Commissioner Android App to scan the Connect QR Code</b></h6>' +
                    '  <div layout="row">' +
                    '  <img ng-src="' + image + '" alt="qr code" style="display: block; margin-left: auto; margin-right: auto; width:40%"></img>' +
                    '  </div>' +
                    '  <md-dialog-actions>' +
                    '    <md-button ng-click="closeDialog()" class="md-primary">' +
                    '      Close' +
                    '    </md-button>' +
                    '  </md-dialog-actions>' +
                    '</md-dialog>',
                   controller: function DialogController($scope, $mdDialog) {
                    $scope.closeDialog = function() {
                    $mdDialog.hide();
                    }
                },
                    multiple: true
             });
            };

            $scope.join = function(valid) {
                if (!valid)
                {
                    return;
                }

                if ($scope.thread.defaultRoute == null) {
                    $scope.thread.defaultRoute = false;
                };
                $scope.isDisplay = true;
                var data = {
                    credentialType: $scope.thread.credentialType,
                    networkKey: $scope.thread.networkKey,
                    pskd: $scope.thread.pskd,
                    prefix: $scope.thread.prefix,
                    defaultRoute: $scope.thread.defaultRoute,
                    index: index,
                };
                var httpRequest = $http({
                    method: 'POST',
                    url: 'join_network',
                    data: data,
                });

                httpRequest.then(function successCallback(response) {
                    $scope.res = response.data.result;
                    if (response.data.result == 'successful') {
                        $mdDialog.hide();
                    }
                    $scope.isDisplay = false;
                    $scope.showAlert(event, "Join operation is " + response.data.result + ". " + response.data.message);
                });
            };

            $scope.cancel = function() {
                $mdDialog.cancel();
            };

            $scope.qrcode = function() {
                $scope.isLoading = false;
                $http.get('get_qrcode').then(function(response) {
                    console.log(response);
                    $scope.res = response.data.result;
                    if (response.data.result == 'successful') {
                        var image = "http://api.qrserver.com/v1/create-qr-code/?color=000000&amp;bgcolor=FFFFFF&amp;data=v%3D1%26%26eui%3D" + response.data.eui64 +"%26%26cc%3D" + $scope.thread.pskd +"&amp;qzone=1&amp;margin=0&amp;size=400x400&amp;ecc=L";
                        $scope.showQRCode(event, image);
                    } else {
                        $scope.showQRAlert(event, "sorry, can not generate the QR code.");
                    }   
                    $scope.isDisplay = true;       
                    
                });
            };
        };


        $scope.showConfirm = function(ev, valid) {
            if (!valid)
            {
                return;
            }

            var confirm = $mdDialog.confirm()
                .title('Are you sure you want to Form the Thread Network?')
                .textContent('')
                .targetEvent(ev)
                .ok('Okay')
                .cancel('Cancel');

            $mdDialog.show(confirm).then(function() {
                if ($scope.thread.defaultRoute == null) {
                    $scope.thread.defaultRoute = false;
                };
                var data = {
                    networkKey: $scope.thread.networkKey,
                    prefix: $scope.thread.prefix,
                    defaultRoute: $scope.thread.defaultRoute,
                    extPanId: $scope.thread.extPanId,
                    panId: $scope.thread.panId,
                    passphrase: $scope.thread.passphrase,
                    channel: $scope.thread.channel,
                    networkName: $scope.thread.networkName,
                };
                $scope.isForming = true;
                var httpRequest = $http({
                    method: 'POST',
                    url: 'form_network',
                    data: data,
                });

                httpRequest.then(function successCallback(response) {
                    $scope.res = response.data.result;
                    if (response.data.result == 'successful') {
                        $mdDialog.hide();
                    }
                    $scope.isForming = false;
                    $scope.showAlert(event, 'FORM', response.data.result);
                });
            }, function() {
                $mdDialog.cancel();
            });
        };

        $scope.showAlert = function(ev, operation, result) {
            $mdDialog.show(
                $mdDialog.alert()
                .parent(angular.element(document.querySelector('#popupContainer')))
                .clickOutsideToClose(true)
                .title('Information')
                .textContent(operation + ' operation is ' + result)
                .ariaLabel('Alert Dialog Demo')
                .ok('Okay')
                .targetEvent(ev)
            );
        };

        $scope.showAddConfirm = function(ev) {
            var confirm = $mdDialog.confirm()
                .title('Are you sure you want to Add this On-Mesh Prefix?')
                .textContent('')
                .targetEvent(ev)
                .ok('Okay')
                .cancel('Cancel');

            $mdDialog.show(confirm).then(function() {
                if ($scope.setting.defaultRoute == null) {
                    $scope.setting.defaultRoute = false;
                };
                var data = {
                    prefix: $scope.setting.prefix,
                    defaultRoute: $scope.setting.defaultRoute,
                };
                var httpRequest = $http({
                    method: 'POST',
                    url: 'add_prefix',
                    data: data,
                });

                httpRequest.then(function successCallback(response) {
                    $scope.showAlert(event, 'Add', response.data.result);
                });
            }, function() {
                $mdDialog.cancel();
            });
        };

        $scope.showDeleteConfirm = function(ev) {
            var confirm = $mdDialog.confirm()
                .title('Are you sure you want to Delete this On-Mesh Prefix?')
                .textContent('')
                .targetEvent(ev)
                .ok('Okay')
                .cancel('Cancel');

            $mdDialog.show(confirm).then(function() {
                var data = {
                    prefix: $scope.setting.prefix,
                };
                var httpRequest = $http({
                    method: 'POST',
                    url: 'delete_prefix',
                    data: data,
                });

                httpRequest.then(function successCallback(response) {
                    $scope.showAlert(event, 'Delete', response.data.result);
                });
            }, function() {
                $mdDialog.cancel();
            });
        };

        $scope.startCommission = function(ev) {
            var data = {
                pskd: $scope.commission.pskd,
                passphrase: $scope.commission.passphrase,
            };
            var httpRequest = $http({
                method: 'POST',
                url: 'commission',
                data: data,
            });
            
            ev.target.disabled = true;
            
            httpRequest.then(function successCallback(response) {
                if (response.data.error == 0) {
                    $scope.showAlert(event, 'Commission', 'success');
                } else {
                    $scope.showAlert(event, 'Commission', 'failed');
                }
                ev.target.disabled = false;
            });
        };

        $scope.restServerPort = '8081';
        $scope.ipAddr = window.location.hostname + ':' + $scope.restServerPort;

        // Basic information line
        $scope.basicInfo = {
            'NetworkName' : 'Unknown',
            'LeaderData'  :{'LeaderRouterId' : 'Unknown'}
        }
        // Num of router calculated by diagnostic
        $scope.NumOfRouter = 'Unknown';

        // Diagnostic information for detailed display
        $scope.nodeDetailInfo = 'Unknown';
        // For response of Diagnostic
        $scope.networksDiagInfo = '';
        $scope.graphisReady = false;
        $scope.detailList = {
            'ExtAddress': { 'title': false, 'content': true },
            'Rloc16': { 'title': false, 'content': true },
            'Mode': { 'title': false, 'content': false },
            'Connectivity': { 'title': false, 'content': false },
            'Route': { 'title': false, 'content': false },
            'LeaderData': { 'title': false, 'content': false },
            'NetworkData': { 'title': false, 'content': true },
            'IP6Address List': { 'title': false, 'content': true },
            'MACCounters': { 'title': false, 'content': false },
            'ChildTable': { 'title': false, 'content': false },
            'ChannelPages': { 'title': false, 'content': false }
        };
        $scope.graphInfo = {
            'nodes': [],
            'links': []
        }

        $scope.dataInit = function() {

            $http.get('http://' + $scope.ipAddr + '/node').then(function(response) {

                $scope.basicInfo = response.data;
                console.log(response.data);
                $scope.basicInfo.Rloc16 = $scope.intToHexString($scope.basicInfo.Rloc16,4);
                $scope.basicInfo.LeaderData.LeaderRouterId = '0x' + $scope.intToHexString($scope.basicInfo.LeaderData.LeaderRouterId,2);
            });
        }
        $scope.isObject = function(obj) {
            return obj.constructor === Object;
        }
        $scope.isArray = function(arr) {
            return !!arr && arr.constructor === Array;
        }

        $scope.clickList = function(key) {
            $scope.detailList[key]['content'] = !$scope.detailList[key]['content']
        }

        $scope.intToHexString = function(num, len){
            var value;
            value  = num.toString(16);
            
            while( value.length < len ){
                value = '0' + value;
            }
            return value;
        }
        $scope.showTopology = function() {
            var nodeMap = {}
            var count, src, dist, rloc, child, rlocOfParent, rlocOfChild, diagOfNode, linkNode, childInfo;

            $scope.graphisReady = false;
            $scope.graphInfo = {
                'nodes': [],
                'links': []
            };
            $http.get('http://' + $scope.ipAddr + '/diagnostics').then(function(response) {

                
                $scope.networksDiagInfo = response.data;
                for (diagOfNode of $scope.networksDiagInfo){
                    
                    diagOfNode['RouteId'] = '0x' + $scope.intToHexString(diagOfNode['Rloc16'] >> 10,2);
                    
                    diagOfNode['Rloc16'] = '0x' + $scope.intToHexString(diagOfNode['Rloc16'],4);
                    
                    diagOfNode['LeaderData']['LeaderRouterId'] = '0x' + $scope.intToHexString(diagOfNode['LeaderData']['LeaderRouterId'],2);
                    for (linkNode of diagOfNode['Route']['RouteData']){
                        linkNode['RouteId'] = '0x' + $scope.intToHexString(linkNode['RouteId'],2);
                    }
                }
                
                count = 0;
                
                for (diagOfNode of $scope.networksDiagInfo) {
                    if ('ChildTable' in diagOfNode) {
                        
                        rloc = parseInt(diagOfNode['Rloc16'],16).toString(16);
                        nodeMap[rloc] = count;
                        
                        if ( diagOfNode['RouteId'] == diagOfNode['LeaderData']['LeaderRouterId']) {
                            diagOfNode['Role'] = 'Leader';
                        } else {
                            diagOfNode['Role'] = 'Router';
                        }

                        $scope.graphInfo.nodes.push(diagOfNode);
                        
                        if (diagOfNode['Rloc16'] === $scope.basicInfo.rloc16) {
                            $scope.nodeDetailInfo = diagOfNode
                        }
                        count = count + 1;
                    }
                }
                // Num of Router is based on the diagnostic information
                $scope.NumOfRouter = count;
                
                // Index for a second loop
                src = 0;
                // Construct links 
                for (diagOfNode of $scope.networksDiagInfo) {
                    if ('ChildTable' in diagOfNode) {
                        // Link bewtwen routers
                        for (linkNode of diagOfNode['Route']['RouteData']) {
                            rloc = ( parseInt(linkNode['RouteId'],16) << 10).toString(16);
                            if (rloc in nodeMap) {
                                dist = nodeMap[rloc];
                                if (src < dist) {
                                    $scope.graphInfo.links.push({
                                        'source': src,
                                        'target': dist,
                                        'weight': 1,
                                        'type': 0,
                                        'linkInfo': {
                                            'inQuality': linkNode['LinkQualityIn'],
                                            'outQuality': linkNode['LinkQualityOut']
                                        }
                                    });
                                }
                            }
                        }

                        // Link between router and child 
                        for (childInfo of diagOfNode['ChildTable']) {
                            child = {};
                            rlocOfParent = parseInt(diagOfNode['Rloc16'],16).toString(16);
                            rlocOfChild = (parseInt(diagOfNode['Rloc16'],16) + childInfo['ChildId']).toString(16);

                            src = nodeMap[rlocOfParent];
                            
                            child['Rloc16'] = '0x' + rlocOfChild;
                            child['RouteId'] = diagOfNode['RouteId'];
                            nodeMap[rlocOfChild] = count;
                            child['Role'] = 'Child';
                            $scope.graphInfo.nodes.push(child);
                            $scope.graphInfo.links.push({
                                'source': src,
                                'target': count,
                                'weight': 1,
                                'type': 1,
                                'linkInfo': {
                                    'Timeout': childInfo['Timeout'],
                                    'Mode': childInfo['Mode']
                                }

                            });

                            count = count + 1;
                        }
                    }
                    src = src + 1;
                }
               
                $scope.drawGraph();
            })
        }

        
        $scope.updateDetailLabel = function() {
            for (var detailInfoKey in $scope.detailList) {
                $scope.detailList[detailInfoKey]['title'] = false;
            }
            for (var diagInfoKey in $scope.nodeDetailInfo) {
                if (diagInfoKey in $scope.detailList) {
                    $scope.detailList[diagInfoKey]['title'] = true;
                }

            }
        }

        
        $scope.drawGraph = function() {
            var json, svg, tooltip, force;
            var scale, len;

            document.getElementById('topograph').innerHTML = '';
            scale = $scope.graphInfo.nodes.length;
            len = 125 * Math.sqrt(scale);

            // Topology graph
            svg = d3.select('.d3graph').append('svg')
                .attr('preserveAspectRatio', 'xMidYMid meet')
                .attr('viewBox', '0, 0, ' + len.toString(10) + ', ' + (len / (3 / 2)).toString(10));
            
            // Legend
            svg.append('circle')
                .attr('cx',len-20)
                .attr('cy',10).attr('r', 3)
                .style('fill', "#7e77f8")
                .style('stroke', '#484e46')
                .style('stroke-width', '0.4px');
            
            svg.append('circle')
                .attr("cx",len-20)
                .attr('cy',20)
                .attr('r', 3)
                .style('fill', '#03e2dd')
                .style('stroke', '#484e46')
                .style('stroke-width', '0.4px');
            
            svg.append('circle')
                .attr('cx',len-20)
                .attr('cy',30)
                .attr('r', 3)
                .style('fill', '#aad4b0')
                .style('stroke', '#484e46')
                .style('stroke-width', '0.4px')
                .style('stroke-dasharray','2 1');
           
            svg.append('circle')
                .attr('cx',len-50)
                .attr('cy',10).attr('r', 3)
                .style('fill', '#ffffff')
                .style('stroke', '#f39191')
                .style('stroke-width', '0.4px');
            
            svg.append('text')
                .attr('x', len-15)
                .attr('y', 10)
                .text('Leader')
                .style('font-size', '4px')
                .attr('alignment-baseline','middle');
            
            svg.append('text')
                .attr('x', len-15)
                .attr('y',20 )
                .text('Router')
                .style('font-size', '4px')
                .attr('alignment-baseline','middle');
            
            svg.append('text')
                .attr('x', len-15)
                .attr('y',30 )
                .text('Child')
                .style('font-size', '4px')
                .attr('alignment-baseline','middle');
            
            svg.append('text')
                .attr('x', len-45)
                .attr('y',10 )
                .text('Selected')
                .style('font-size', '4px')
                .attr('alignment-baseline','middle');

            // Tooltip style  for each node
            tooltip = d3.select('body')
                .append('div')
                .attr('class', 'tooltip')
                .style('position', 'absolute')
                .style('z-index', '10')
                .style('visibility', 'hidden')
                .text('a simple tooltip');

            force = d3.layout.force()
                .distance(40)
                .size([len, len / (3 / 2)]);

            
            json = $scope.graphInfo;
           
            force
                .nodes(json.nodes)
                .links(json.links)
                .start();


            var link = svg.selectAll('.link')
                .data(json.links)
                .enter().append('line')
                .attr('class', 'link')
                .style('stroke', '#908484')
                // Dash line for link between child and parent
                .style('stroke-dasharray', function(item) {
                    if ('Timeout' in item.linkInfo) return '4 4';
                    else return '0 0'
                })
                // Line width representing link quality
                .style('stroke-width', function(item) {
                    if ('inQuality' in item.linkInfo)
                        return Math.sqrt(item.linkInfo.inQuality/2);
                    else return Math.sqrt(0.5)
                })
                // Effect of mouseover on a line
                .on('mouseover', function(item) {
                    return tooltip.style('visibility', 'visible')
                        .text(item.linkInfo);
                })
                .on('mousemove', function() {
                    return tooltip.style('top', (d3.event.pageY - 10) + 'px')
                        .style('left', (d3.event.pageX + 10) + 'px');
                })
                .on('mouseout', function() {
                    return tooltip.style('visibility', 'hidden');
                });


            var node = svg.selectAll('.node')
                .data(json.nodes)
                .enter().append('g')
                .attr('class', function(item) {
                        return item.Role;
                })
                .call(force.drag)
                // Tooltip effect of mouseover on a node 
                .on('mouseover', function(item) {
                    return tooltip.style('visibility', 'visible')
                                  .text(item.Rloc16 );
                })
                .on('mousemove', function() {
                    return tooltip.style('top', (d3.event.pageY - 10) + 'px')
                                  .style('left', (d3.event.pageX + 10) + 'px');
                })
                .on('mouseout', function() {
                    return tooltip.style('visibility', 'hidden');
                });

            d3.selectAll('.Child')
                .append('circle')
                .attr('r', '6')
                .attr('fill', '#aad4b0')
                .style('stroke', '#484e46')
                .style('stroke-dasharray','2 1')
                .style('stroke-width', '0.5px')
                .attr('class', function(item) {
                    return item.Rloc16;
                })
                .on('mouseover', function(item) {
                    return tooltip.style('visibility', 'visible')
                                  .text(item.Rloc16 );
                })
                .on('mousemove', function() {
                    return tooltip.style('top', (d3.event.pageY - 10) + 'px')
                                  .style('left', (d3.event.pageX + 10) + 'px');
                })
                .on('mouseout', function() {
                    return tooltip.style('visibility', 'hidden');
                });


            d3.selectAll('.Leader')
                .append('circle')
                .attr('r', '8')
                .attr('fill', '#7e77f8')
                .style('stroke', '#484e46')
                .style('stroke-width', '1px')
                .attr('class', function(item) {
                    return 'Stroke';
                })
                // Effect that node will become bigger when mouseover
                .on('mouseover', function(item) {
                    d3.select(this)
                        .transition()
                        .attr('r','9');
                    return tooltip.style('visibility', 'visible')
                                  .text(item.Rloc16);
                })
                .on('mousemove', function() {
                    return tooltip.style('top', (d3.event.pageY - 10) + 'px')
                                  .style('left', (d3.event.pageX + 10) + 'px');
                })
                .on('mouseout', function() {
                    d3.select(this).transition().attr('r','8');
                        return tooltip.style('visibility', 'hidden');
                })
                // Effect that node will have a yellow edge when clicked
                .on('click', function(item) {
                    d3.selectAll('.Stroke')
                        .style('stroke', '#484e46')
                        .style('stroke-width', '1px');
                    d3.select(this)
                        .style('stroke', '#f39191')
                        .style('stroke-width', '1px');
                    $scope.$apply(function() {
                        $scope.nodeDetailInfo = item;
                        $scope.updateDetailLabel();
                    });
                });
            d3.selectAll('.Router')
                .append('circle')
                .attr('r', '8')
                .style('stroke', '#484e46')
                .style('stroke-width', '1px')
                .attr('fill', '#03e2dd')
                .attr('class','Stroke')
                .on('mouseover', function(item) {
                    d3.select(this)
                        .transition()
                        .attr('r','8');
                    return tooltip.style('visibility', 'visible')
                                  .text(item.Rloc16);
                })
                .on('mousemove', function() {
                    return tooltip.style('top', (d3.event.pageY - 10) + 'px')
                                  .style('left', (d3.event.pageX + 10) + 'px');
                })
                .on('mouseout', function() {
                    d3.select(this)
                        .transition()
                        .attr('r','7');
                    return tooltip.style('visibility', 'hidden');
                })
                // The same effect as Leader
                .on('click', function(item) {
                    d3.selectAll('.Stroke')
                        .style('stroke', '#484e46')
                        .style('stroke-width', '1px');
                    d3.select(this)
                        .style('stroke', '#f39191')
                        .style('stroke-width', '1px');
                    $scope.$apply(function() {
                        $scope.nodeDetailInfo = item;
                        $scope.updateDetailLabel();
                    });
                });

            force.on('tick', function() {
                link.attr('x1', function(item) { return item.source.x; })
                    .attr('y1', function(item) { return item.source.y; })
                    .attr('x2', function(item) { return item.target.x; })
                    .attr('y2', function(item) { return item.target.y; });
                node.attr('transform', function(item) {
                    return 'translate(' + item.x + ',' + item.y + ')';
                });
            });
            
            $scope.updateDetailLabel();
            $scope.graphisReady = true;

        }
    };
})();
