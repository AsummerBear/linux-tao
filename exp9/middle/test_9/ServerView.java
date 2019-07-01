package com.test_9;

import javax.swing.*;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.awt.event.WindowAdapter;
import java.awt.event.WindowEvent;
import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStreamReader;
import java.io.PrintWriter;
import java.net.BindException;
import java.net.ServerSocket;
import java.net.Socket;
import java.util.ArrayList;
import java.util.StringTokenizer;

public class ServerView {

    private JFrame frame;
    private JTextField maxNum;
    private JTextField portNum;
    private JButton control;
    private JTextArea msgList;
    private JTextField msgtext;
    private JButton sendButton;
    private JPanel panel;
    private JList userList;

    private DefaultListModel listModel;

    private ServerSocket serverSocket;
    private ServerThread serverThread;
    private ArrayList<ClientThread> clients;

    private boolean isStart = false;

    public static void main(String[] args) {
        new ServerView();
    }

    public ServerView(){
        frame = new JFrame("ServerView");
        listModel = new DefaultListModel();
        userList.setModel(listModel);
        frame.setContentPane(panel);
        frame.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
        frame.pack();
        frame.setSize(600,400);
        frame.setVisible(true);
        // 关闭窗口时事件
        frame.addWindowListener(new WindowAdapter() {
            public void windowClosing(WindowEvent e) {
                if (isStart) {
                    closeServer();// 关闭服务器
                }
                System.exit(0);// 退出程序
            }
        });

        // 文本框按回车键时事件
        msgtext.addActionListener(new ActionListener() {
            public void actionPerformed(ActionEvent e) {
                send();
            }
        });

        // 单击发送按钮时事件
        sendButton.addActionListener(new ActionListener() {
            public void actionPerformed(ActionEvent arg0) {
                send();
            }
        });

        // 单击控制服务器按钮时事件
        control.addActionListener(new ActionListener() {
            public void actionPerformed(ActionEvent e) {
                if (isStart) {
                    try {
                        closeServer();
                        maxNum.setEnabled(true);
                        portNum.setEnabled(true);
                        control.setText("启动服务器");
                        msgList.append("服务器成功停止!\r\n");
                        JOptionPane.showMessageDialog(frame, "服务器成功停止！");
                    } catch (Exception exc) {
                        JOptionPane.showMessageDialog(frame, "停止服务器发生异常！", "错误",
                                JOptionPane.ERROR_MESSAGE);
                    }
                }else {
                    int max;
                    int port;
                    try {
                        try {
                            max = Integer.parseInt(maxNum.getText());
                        } catch (Exception e1) {
                            throw new Exception("人数上限为正整数！");
                        }
                        if (max <= 0) {
                            throw new Exception("人数上限为正整数！");
                        }
                        try {
                            port = Integer.parseInt(portNum.getText());
                        } catch (Exception e1) {
                            throw new Exception("端口号为正整数！");
                        }
                        if (port <= 0) {
                            throw new Exception("端口号 为正整数！");
                        }
                        serverStart(max, port);
                        msgList.append("服务器已成功启动!人数上限：" + max + ",端口：" + port
                                + "\r\n");
                        JOptionPane.showMessageDialog(frame, "服务器成功启动!");
                        maxNum.setEnabled(false);
                        portNum.setEnabled(false);
                        control.setText("关闭服务器");
                    } catch (Exception exc) {
                        JOptionPane.showMessageDialog(frame, exc.getMessage(),
                                "错误", JOptionPane.ERROR_MESSAGE);
                    }
                }
            }
        });

    }

    // 执行消息发送
    public void send() {
        if (!isStart) {
            JOptionPane.showMessageDialog(frame, "服务器还未启动,不能发送消息！", "错误",
                    JOptionPane.ERROR_MESSAGE);
            return;
        }
        if (clients.size() == 0) {
            JOptionPane.showMessageDialog(frame, "没有用户在线,不能发送消息！", "错误",
                    JOptionPane.ERROR_MESSAGE);
            return;
        }
        String message = msgtext.getText().trim();
        if (message == null || message.equals("")) {
            JOptionPane.showMessageDialog(frame, "消息不能为空！", "错误",
                    JOptionPane.ERROR_MESSAGE);
            return;
        }
        sendServerMessage(message);// 群发服务器消息
        msgList.append("服务器说：" + msgtext.getText() + "\r\n");
        msgtext.setText(null);
    }

    // 启动服务器
    public void serverStart(int max, int port) throws java.net.BindException {
        try {
            clients = new ArrayList<ClientThread>();
            serverSocket = new ServerSocket(port);
            serverThread = new ServerThread(serverSocket, max);
            serverThread.start();
            isStart = true;
        } catch (BindException e) {
            isStart = false;
            throw new BindException("端口号已被占用，请换一个！");
        } catch (Exception e1) {
            e1.printStackTrace();
            isStart = false;
            throw new BindException("启动服务器异常！");
        }
    }

    // 关闭服务器
    @SuppressWarnings("deprecation")
    public void closeServer() {
        try {
            if (serverThread != null)
                serverThread.stop();// 停止服务器线程

            for (int i = clients.size() - 1; i >= 0; i--) {
                // 给所有在线用户发送关闭命令
                clients.get(i).getWriter().println("CLOSE");
                clients.get(i).getWriter().flush();
                // 释放资源
                clients.get(i).stop();// 停止此条为客户端服务的线程
                clients.get(i).reader.close();
                clients.get(i).writer.close();
                clients.get(i).socket.close();
                clients.remove(i);
            }
            if (serverSocket != null) {
                serverSocket.close();// 关闭服务器端连接
            }
            listModel.removeAllElements();// 清空用户列表
            isStart = false;
        } catch (IOException e) {
            e.printStackTrace();
            isStart = true;
        }
    }

    // 群发服务器消息
    public void sendServerMessage(String message) {
        for (int i = clients.size() - 1; i >= 0; i--) {
            clients.get(i).getWriter().println("服务器：" + message + "(多人发送)");
            clients.get(i).getWriter().flush();
        }
    }

    // 服务器线程
    class ServerThread extends Thread {
        private ServerSocket serverSocket;
        private int max;// 人数上限

        // 服务器线程的构造方法
        public ServerThread(ServerSocket serverSocket, int max) {
            this.serverSocket = serverSocket;
            this.max = max;
        }

        public void run() {
            while (true) {// 不停的等待客户端的链接
                try {
                    Socket socket = serverSocket.accept();
                    if (clients.size() == max) {// 如果已达人数上限
                        BufferedReader r = new BufferedReader(
                                new InputStreamReader(socket.getInputStream()));
                        PrintWriter w = new PrintWriter(socket
                                .getOutputStream());
                        // 接收客户端的基本用户信息
                        String inf = r.readLine();
                        StringTokenizer st = new StringTokenizer(inf, "@");
                        User user = new User(st.nextToken(), st.nextToken());
                        // 反馈连接成功信息
                        w.println("MAX@服务器：对不起，" + user.getName()
                                + user.getIp() + "，服务器在线人数已达上限，请稍后尝试连接！");
                        w.flush();
                        // 释放资源
                        r.close();
                        w.close();
                        socket.close();
                        continue;
                    }
                    ClientThread client = new ClientThread(socket);
                    client.start();// 开启对此客户端服务的线程
                    clients.add(client);
                    listModel.addElement(client.getUser().getName());// 更新在线列表
                    msgList.append(client.getUser().getName()
                            + client.getUser().getIp() + "上线!\r\n");
                } catch (IOException e) {
                    e.printStackTrace();
                }
            }
        }
    }

    // 为一个客户端服务的线程
    class ClientThread extends Thread {
        private Socket socket;
        private BufferedReader reader;
        private PrintWriter writer;
        private User user;

        public BufferedReader getReader() {
            return reader;
        }

        public PrintWriter getWriter() {
            return writer;
        }

        public User getUser() {
            return user;
        }

        // 客户端线程的构造方法
        public ClientThread(Socket socket) {
            try {
                this.socket = socket;
                reader = new BufferedReader(new InputStreamReader(socket
                        .getInputStream()));
                writer = new PrintWriter(socket.getOutputStream());
                // 接收客户端的基本用户信息
                String inf = reader.readLine();
                StringTokenizer st = new StringTokenizer(inf, "@");
                user = new User(st.nextToken(), st.nextToken());
                // 反馈连接成功信息
                writer.println(user.getName() + user.getIp() + "与服务器连接成功!");
                writer.flush();
                // 反馈当前在线用户信息
                if (clients.size() > 0) {
                    String temp = "";
                    for (int i = clients.size() - 1; i >= 0; i--) {
                        temp += (clients.get(i).getUser().getName() + "/" + clients
                                .get(i).getUser().getIp())
                                + "@";
                    }
                    writer.println("USERLIST@" + clients.size() + "@" + temp);
                    writer.flush();
                }
                // 向所有在线用户发送该用户上线命令
                for (int i = clients.size() - 1; i >= 0; i--) {
                    clients.get(i).getWriter().println(
                            "ADD@" + user.getName() + user.getIp());
                    clients.get(i).getWriter().flush();
                }
            } catch (IOException e) {
                e.printStackTrace();
            }
        }

        @SuppressWarnings("deprecation")
        public void run() {// 不断接收客户端的消息，进行处理。
            String message = null;
            while (true) {
                try {
                    message = reader.readLine();// 接收客户端消息
                    if (message.equals("CLOSE"))// 下线命令
                    {
                        msgList.append(this.getUser().getName()
                                + this.getUser().getIp() + "下线!\r\n");
                        // 断开连接释放资源
                        reader.close();
                        writer.close();
                        socket.close();

                        // 向所有在线用户发送该用户的下线命令
                        for (int i = clients.size() - 1; i >= 0; i--) {
                            clients.get(i).getWriter().println(
                                    "DELETE@" + user.getName());
                            clients.get(i).getWriter().flush();
                        }

                        listModel.removeElement(user.getName());// 更新在线列表

                        // 删除此条客户端服务线程
                        for (int i = clients.size() - 1; i >= 0; i--) {
                            if (clients.get(i).getUser() == user) {
                                ClientThread temp = clients.get(i);
                                clients.remove(i);// 删除此用户的服务线程
                                temp.stop();// 停止这条服务线程
                                return;
                            }
                        }
                    } else {
                        dispatcherMessage(message);// 转发消息
                    }
                } catch (IOException e) {
                    e.printStackTrace();
                }
            }
        }

        // 转发消息
        public void dispatcherMessage(String message) {
            StringTokenizer stringTokenizer = new StringTokenizer(message, "@");
            String source = stringTokenizer.nextToken();
            String owner = stringTokenizer.nextToken();
            String content = stringTokenizer.nextToken();
            message = source + "说：" + content;
            msgList.append(message + "\r\n");
            if (owner.equals("ALL")) {// 群发
                for (int i = clients.size() - 1; i >= 0; i--) {
                    clients.get(i).getWriter().println(message + "(多人发送)");
                    clients.get(i).getWriter().flush();
                }
            }
        }
    }
}
